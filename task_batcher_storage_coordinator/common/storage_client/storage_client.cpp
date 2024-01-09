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

bool TStorageClient::UpdateScriptContent(
    const std::string& buildName,
    const std::string& scriptName,
    std::string content
) {
    auto maybeData = impl_.GetData("builds", buildName);

    if (!maybeData.has_value()) {
        return false;
    }

    auto data = nlohmann::json::parse(std::move(maybeData.value()), nullptr, false);

    if (data.is_discarded()) {
        return false;
    }

    data[scriptName]["content"] = std::move(content);
    return impl_.UpsertData("builds", buildName, data.dump());
}

bool TStorageClient::UploadInitScript(const std::string& buildName, std::string content) {
    return UpdateScriptContent(buildName, "initScript", std::move(content));
}

bool TStorageClient::UploadExecuteScript(const std::string& buildName, std::string content) {
    return UpdateScriptContent(buildName, "executeScript", std::move(content));
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

void TStorageClient::UploadTests(
    std::vector<std::string>&& tests,
    const std::string& testSuffix,
    const std::string& taskId
) {
    for (size_t i = 0; i < tests.size(); ++i) {
        std::string fileName = std::to_string(i + 1) + "_" + testSuffix;
        impl_.UpsertData(taskId, fileName, std::move(tests[i]));
    }
}

void TStorageClient::UploadTaskBatches(
    std::vector<std::vector<uint64_t>>&& batches,
    const std::string& taskId,
    size_t batchSize
) {
    nlohmann::json metaData;
    metaData["batchSize"] = batchSize;
    metaData["batches"] = std::move(batches);

    impl_.UpsertData(taskId, "meta.json", metaData.dump());
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

std::optional<std::string> TStorageClient::GetTaskMeta(const std::string &taskId) {
    return impl_.GetData(taskId, "meta.json");
}

} // end of NDTS::NTabasco namespace