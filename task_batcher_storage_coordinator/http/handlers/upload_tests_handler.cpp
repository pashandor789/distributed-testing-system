#include "upload_tests_handler.h"

#include <vector>

#include "utils/parse_json.h"

#include <nlohmann/json.hpp>

namespace NDTS::NTabasco {

bool TUploadTestsHandler::Parse(const crow::request& req, crow::response& res) {
    crow::multipart::message msg(req);

    auto taskDataIt = msg.part_map.find("taskData.json");

    if (taskDataIt == msg.part_map.end()) {
        res.code = 400;
        res.body = "no taskData.json specified";
        return false;
    }

    auto taskData = ParseJSON(std::move(taskDataIt->second.body), {"taskId"});

    if (taskData.HasError()) {
        res.code = 400;
        res.body = std::move(taskData.Error());
        return false;
    }

    size_t testCount = msg.parts.size() / 2;

    std::vector<std::string> inputTests;
    inputTests.reserve(testCount);

    std::vector<std::string> outputTests;
    outputTests.reserve(testCount);

    for (size_t testIndex = 1; testIndex < testCount + 1; ++testIndex) {
        std::string inputTestFileName = std::to_string(testIndex) + "_input";
        auto inputTestIt = msg.part_map.find(inputTestFileName);
        if (inputTestIt == msg.part_map.end()) {
            res.code = 400;
            res.body = "no " + inputTestFileName;
            return false;
        }
        inputTests.push_back(std::move(inputTestIt->second.body));

        std::string outputTestFileName = std::to_string(testIndex) + "_output";
        auto outputTestIt = msg.part_map.find(outputTestFileName);
        if (outputTestIt == msg.part_map.end()) {
            res.code = 400;
            res.body = "no " + outputTestFileName;
            return false;
        }
        outputTests.push_back(std::move(outputTestIt->second.body));
    }


    task_.tests.inputTests = std::move(inputTests);
    task_.tests.outputTests = std::move(outputTests);
    task_.id = taskData.Value()["taskId"];

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
        size_t testSize = inputTests[i].size() + outputTests[i].size();

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

    auto batches = SplitIntoBatches(
        task_.tests.inputTests,
        task_.tests.outputTests,
        ctx.server->batchSize_
    );

    TTaskMetaData taskMetaData;

    taskMetaData.taskId = task_.id;
    taskMetaData.batches = std::move(batches);
    taskMetaData.batchSize = ctx.server->batchSize_;

    if (auto maybeError = ctx.server->storageClient_.UpsertTaskTests(std::move(task_))) {
        res.code = 400;
        res.body = "UpsertTaskTests Error: " + std::move(maybeError.value());
        return;
    }

    if (auto maybeError = ctx.server->storageClient_.UpsertTaskMetaData(std::move(taskMetaData))) {
        res.code = 400;
        res.body = "UpsertTaskMetaData Error: " + std::move(maybeError.value());
        return;
    }
}

} // end of NDTS::TTabasco namespace 
