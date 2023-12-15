#include "server.h"

#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

#include "common/proto/batch.pb.h"

namespace NDTS::TTabasco {

static const std::string INPUT_TEST_SUFFIX = "test";
static const std::string OUTPUT_TEST_SUFFIX = "answer";

static const char TEST_NUM_SEPARATOR = '_';

std::string GetInvalidTests(const std::vector<bool>& initializedTests) {
    std::string invalidTests;

    for (size_t i = 0; i < initializedTests.size(); ++i) {
        if (!initializedTests[i]) {
            invalidTests
                .append(std::to_string(i + 1))
                .append(" ");
        }
    }

    return invalidTests;
}

TTabascoHTTPServer::TTabascoHTTPServer(const TTabascoHTTPServerConfig& config)
    : batchSize_(config.batchSize())
    , baseURL_(config.storageURL(), false)
    , provider_(config.storageAccessKey(), config.storageSecretKey())
    , client_(baseURL_, &provider_)
{
    InitHandlers();

    app_.concurrency(config.serverThreadCount());
    app_.port(config.serverPort());
}

void TTabascoHTTPServer::InitHandlers() {
    CROW_ROUTE(app_, "/loadTests")(
        GetLoadTestsHandler()
    );
}

void TTabascoHTTPServer::Run() {
    app_.run();
}

void TTabascoHTTPServer::UploadStorageTests(
    std::vector<std::string>&& tests,
    const std::string& testSuffix,
    const std::string& taskId
) {
    for (size_t i = 0; i < tests.size(); ++i) {
        size_t testSize = tests[i].size();
        std::stringstream stream(std::move(tests[i]));

        minio::s3::PutObjectArgs args(stream, testSize, 0);

        args.bucket = taskId;
        args.object = std::to_string(i + 1) + TEST_NUM_SEPARATOR + testSuffix;

        minio::s3::PutObjectResponse resp = client_.PutObject(args);
    }
}

void UploadStorageBatches(
    std::vector<std::vector<uint64_t>&& batches
    const std::string& taskId
) {
    nlohmann::json jsonData["batches"] = std::move(batches);

    std::stringstream stream(jsonData.dump());
    minio::s3::PutObjectArgs args(stream, stream.str().size(), 0);

    args.bucket = taskId;
    args.object = "meta.json";

    minio::s3::PutObjectResponse resp = client_.PutObject(args);
}

TBatches TTabascoHTTPServer::SplitIntoBatches(
    const std::vector<std::string>& inputTests,
    const std::vector<std::string>& outputTests
) {
    size_t batchSize = batchSize_;
    size_t testsCount = inputTests.size();

    for (size_t i = 0; i < testsCount; ++i) {
        batchSize = std::max(batchSize, inputTests[i].size() + outputTests[i].size());
    }

    size_t accumSize = 0;

    std::vector<std::vector<size_t>> batches;
    batches.push_back({});

    for (size_t i = 0; i < testsCount; ++i) {
        size_t testSize = inputTests.size() + outputTests.size();

        if (accumSize + testSize > batchSize) {
            batches.push_back({});
            accumSize = 0;
        }

        batches.back().push_back(i + 1);
        accumSize += testSize;
    }

    return batches;
}

auto TTabascoHTTPServer::GetLoadTestsHandler() {
    auto handler = [this](const crow::request& req, crow::response& res) {
        if (req.get_header_value("Content-Type") != "multipart/form-data") {
            res.code = 400;
            return;
        }

        std::string taskId = "";

        crow::multipart::message msg(req);

        size_t testCount = msg.parts.size();

        std::vector<std::string> inputTests(testCount);
        std::vector<std::string> outputTests(testCount);

        std::vector<bool> initializedInputTests(testCount, false);
        std::vector<bool> initialziedOutputTests(testCount, false);

        for (auto& part: msg.parts) {
            auto contentDisposition = part.get_header_object("Content-Disposition").params;
        
            if (contentDisposition["name"] == "taskId") {
                taskId = std::move(part.body());
                continue;
            }

            if (!contentDisposition.contains("filename")) {
                continue;
            }

            int testNum = -1;
            std::string testSuffix = "";

            std::stringstream stream(contentDisposition["filename"]);
            
            stream >> testNum;
            stream.ignore(1, TEST_NUM_SEPARATOR);
            stream >> testSuffix;

            std::vector<std::string>* tests = nullptr;

            if (testSuffix == INPUT_TEST_SUFFIX) {
                initializedInputTests[testNum - 1] = true;
                tests = &inputTests;
            }

            if (testSuffix == OUTPUT_TEST_SUFFIX) {
                initialziedOutputTests[testNum - 1] = true;
                tests = &outputTests;
            }

            if (tests == nullptr || !(1 <= testNum && testNum <= testCount)) {
                res.code = 400;
                std::string errorString;

                errorString
                    .append("bad format: ")
                    .append(testSuffix)
                    .append(" for ")
                    .append(contentDisposition["filename"]);

                res.body = errorString;
                return;
            }

            (*tests)[testNum - 1] = std::move(part.body);
        }

        auto invalidInputTests = GetInvalidTests(initializedInputTests);
        auto invalidOutputTests = GetInvalidTests(initialziedOutputTests);

        if (!(invalidInputTests.empty() && invalidOutputTests.empty())) {
            res.code = 400;
            std::string errorString;

            errorString
                .append("not found tests input: ")
                .append(invalidInputTests)
                .append(" output ")
                .append(invalidOutputTests);

            res.body = errorString;
            return;
        }

        auto batches = this->SplitIntoBatches(inputTests, outputTests);

        this->UploadStorageBatches(std::move(batches), taskId);

        this->UploadStorageTests(std::move(inputTests), INPUT_TEST_SUFFIX, taskId);
        this->UploadStorageTests(std::move(outputTests), OUTPUT_TEST_SUFFIX, taskId);
    };

    return handler;
}

} // end of NDTS::TTabasco namespace
