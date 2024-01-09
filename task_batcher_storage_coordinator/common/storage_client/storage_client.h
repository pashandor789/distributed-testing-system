#pragma once

#include "common/proto/storage_client.pb.h"
#include "impl/storage_client_impl.h"
#include "data_repr.h"

namespace NDTS::NTabasco {

class TStorageClient {
public:
    TStorageClient(const TStorageClientConfig& config);

    bool UploadInitScript(const std::string& buildName, std::string content);

    bool UploadExecuteScript(const std::string& buildName, std::string content);

    bool CreateBuild(TBuild build);

    std::optional<TBuild> GetBuild(const std::string& buildName);

    TBuilds GetBuilds();

    bool CreateTask(const std::string& taskId);

    void UploadTests(
        std::vector<std::string>&& tests,
        const std::string& testSuffix,
        const std::string& taskId
    );

    void UploadTaskBatches(
        std::vector<std::vector<uint64_t>>&& batches,
        const std::string& taskId,
        size_t batchSize
    );

    std::optional<std::string> GetTest(
        const std::string& taskId,
        const std::string& testIndex,
        const std::string& testSuffix
    );

    std::optional<std::string> GetTaskMeta(const std::string& taskId);

private:
    bool UpdateScriptContent(const std::string& buildName, const std::string& scriptName, std::string content);
private:
    TStorageClientImpl impl_;
};

} // end of NDTS::NTabasco namespace