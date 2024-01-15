#include "upload_tests_handler.h"

#include <vector>

#include <nlohmann/json.hpp>

namespace NDTS::NTabasco {

static const std::string INPUT_TEST_SUFFIX = "input";
static const std::string OUTPUT_TEST_SUFFIX = "output";

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

bool TUploadTestsHandler::Parse(const crow::request& req, crow::response& res) {
    crow::multipart::message msg(req);

    size_t testCount = msg.parts.size() / 2;

    std::vector<std::string> inputTests(testCount);
    std::vector<std::string> outputTests(testCount);

    std::vector<bool> initializedInputTests(testCount, false);
    std::vector<bool> initializedOutputTests(testCount, false);

    uint64_t taskId;

    for (auto& part: msg.parts) {
        auto contentDisposition = part.get_header_object("Content-Disposition").params;

        if (contentDisposition["name"] == "taskId") {
            taskId = std::stoi(part.body);
            continue;
        }

        if (!contentDisposition.contains("filename")) {
            continue;
        }

        int testNum = -1;
        std::string testSuffix;

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
            initializedOutputTests[testNum - 1] = true;
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
            return false;
        }

        (*tests)[testNum - 1] = std::move(part.body);
    }

    auto invalidInputTests = GetInvalidTests(initializedInputTests);
    auto invalidOutputTests = GetInvalidTests(initializedOutputTests);

    if (!(invalidInputTests.empty() && invalidOutputTests.empty())) {
        res.code = 400;
        std::string errorString;

        errorString
            .append("not found tests input: ")
            .append(invalidInputTests)
            .append(" output ")
            .append(invalidOutputTests);

        res.body = errorString;
        return false;
    }

    inputTests_ = std::move(inputTests);
    outputTests_ = std::move(outputTests);
    taskId_ = taskId;

    return true;
}

std::vector<std::vector<size_t>> SplitIntoBatches(
    const std::vector<std::string>& inputTests,
    const std::vector<std::string>& outputTests,
    size_t batchSize
) {
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

void TUploadTestsHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    if (!Parse(req, res)) {
        return;
    }

    auto batches = SplitIntoBatches(inputTests_, outputTests_, ctx.server->batchSize_);

    ctx.server->storageClient_.CreateTask(taskId_);

    ctx.server->storageClient_.UploadTaskBatches(
        std::move(batches),
        taskId_,
        ctx.server->batchSize_
    );

    ctx.server->storageClient_.UploadTests(
        std::move(inputTests_),
        std::move(outputTests_),
        taskId_
    );
}

} // end of NDTS::TTabasco namespace 
