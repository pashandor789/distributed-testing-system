#include "storage_client.h"

namespace NDTS::NTabasco {

TStorageClient::TStorageClient(const TStorageClientConfig& config)
    : impl_(config)
{}

bool TStorageClient::CreateBuild(TBuild build) {
    auto buildName = build.name;
    auto deserializedBuildData = build.MoveToJSON().dump();

    return impl_.InsertData("builds", buildName, std::move(deserializedBuildData));
}

bool TStorageClient::UpdateBuild(TBuild build) {
    auto buildName = build.name;
    auto deserializedBuildData = build.MoveToJSON().dump();

    return impl_.UpdateData("builds", buildName, std::move(deserializedBuildData));
}

std::optional<TBuild> TStorageClient::GetBuild(const std::string& buildName) {
    auto maybeData = impl_.GetData("builds", buildName);

    if (!maybeData.has_value()) {
        return std::nullopt;
    }

    auto data = nlohmann::json::parse(std::move(maybeData.value()), nullptr, false);

    return TBuild::FromJSON(std::move(data));
}

bool TStorageClient::CreateTask(const std::string& taskId) {
    return impl_.CreateBucket(taskId);
}

bool TStorageClient::UploadTests(
    std::vector<std::string>&& inputTests,
    std::vector<std::string>&& outputTests,
    const std::string& taskId
) {
    size_t size = inputTests.size();

    for (size_t i = 0; i < size; ++i) {
        std::string inputTestFileName = std::to_string(i + 1) + "_input";

        if (!impl_.UpsertData(taskId, inputTestFileName, std::move(inputTests[i]))) {
            return false;
        }

        std::string outputTestFileName = std::to_string(i + 1) + "_output";

        if (!impl_.UpsertData(taskId, outputTestFileName, std::move(outputTests[i]))) {
            return false;
        }
    }

    return true;
}

bool TStorageClient::UploadTaskRootDir(const std::string& taskId, std::string zipData) {
    return impl_.UpsertData(taskId, "root_dir.zip", std::move(zipData));
}

bool TStorageClient::UploadTaskBatches(
    std::vector<std::vector<uint64_t>>&& batches,
    const std::string& taskId,
    size_t batchSize
) {
    nlohmann::json metaData;
    metaData["batchSize"] = batchSize;
    metaData["batches"] = std::move(batches);

    return impl_.UpsertData(taskId, "meta.json", metaData.dump());
}

TBuilds TStorageClient::GetBuilds() {
    auto foundBuilds = impl_.GetFiles("builds");

    TBuilds builds;
    builds.items.reserve(foundBuilds.size());

    for (auto& file: foundBuilds) {
        auto data = nlohmann::json::parse(std::move(file.content), nullptr, false);
        builds.items.push_back(TBuild::FromJSON(std::move(data)));
    }

    return builds;
}

std::optional<std::string> TStorageClient::GetTest(
    const std::string& taskId,
    const std::string& testIndex,
    const std::string& testSuffix
) {
    return impl_.GetData(taskId, testIndex + "_" + testSuffix);
}

std::optional<std::string> TStorageClient::GetTaskMeta(const std::string& taskId) {
    return impl_.GetData(taskId, "meta.json");
}

bool TStorageClient::CreateChecker(const std::string& checkerName, std::string checkerData) {
    return impl_.InsertData("checkers", checkerName, std::move(checkerData));
}

bool TStorageClient::UpdateChecker(const std::string& checkerName, std::string checkerData) {
    return impl_.UpsertData("checkers", checkerName, std::move(checkerData));
}

std::optional<std::string> TStorageClient::GetCheckerData(const std::string& checkerName) {
    return  impl_.GetData("checkers", checkerName);
}

std::optional<std::string> TStorageClient::GetTaskRootDir(const std::string &taskId) {
    return impl_.GetData(taskId, "root_dir.zip");
}

} // end of NDTS::NTabasco namespace
