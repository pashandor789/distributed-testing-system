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

    bool CreateTask(uint64_t taskId);

    bool UploadTests(
        std::vector<std::string>&& inputTests,
        std::vector<std::string>&& outputTests,
        uint64_t taskId
    );

    bool UploadTaskBatches(
        std::vector<std::vector<uint64_t>>&& batches,
        uint64_t taskId,
        size_t batchSize
    );

    bool UploadTaskRootDir(uint64_t taskId, std::string zipData);

    std::optional<std::string> GetTest(
        uint64_t taskId,
        const std::string& testIndex,
        const std::string& testSuffix
    );

    bool CreateChecker(const std::string& checkerName, std::string checkerData);

    bool UpdateChecker(const std::string& checkerName, std::string checkerData);

    std::optional<std::string> GetTaskMeta(uint64_t taskId);

    std::optional<std::string> GetCheckerData(const std::string& checkerName);

    std::optional<std::string> GetTaskRootDir(uint64_t taskId);

private:
    TStorageClientImpl impl_;
};

} // end of NDTS::NTabasco namespace