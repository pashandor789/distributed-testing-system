#pragma once

#include "common/proto/storage_client.pb.h"
#include "impl/storage_client_impl.h"
#include "data_repr.h"

namespace NDTS::NTabasco {

class TStorageClient {
public:
    TStorageClient(const TStorageClientConfig& config);

    bool CreateBuild(TBuild build);

    bool UpdateBuild(TBuild build);

    std::optional<TBuild> GetBuild(const std::string& buildName);

    TBuilds GetBuilds();

    bool CreateTask(const std::string& taskId);

    bool UploadTests(
        std::vector<std::string>&& inputTests,
        std::vector<std::string>&& outputTests,
        const std::string& taskId
    );

    bool UploadTaskBatches(
        std::vector<std::vector<uint64_t>>&& batches,
        const std::string& taskId,
        size_t batchSize
    );

    bool UploadTaskRootDir(const std::string& taskId, std::string zipData);

    std::optional<std::string> GetTest(
        const std::string& taskId,
        const std::string& testIndex,
        const std::string& testSuffix
    );

    bool CreateChecker(const std::string& checkerName, std::string checkerData);

    bool UpdateChecker(const std::string& checkerName, std::string checkerData);

    std::optional<std::string> GetTaskMeta(const std::string& taskId);

    std::optional<std::string> GetCheckerData(const std::string& checkerName);

    std::optional<std::string> GetTaskRootDir(const std::string& taskId);

private:
    TStorageClientImpl impl_;
};

} // end of NDTS::NTabasco namespace