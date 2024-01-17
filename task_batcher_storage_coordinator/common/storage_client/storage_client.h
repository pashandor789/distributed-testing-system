#pragma once

#include <optional>
#include <string>
#include <memory>

#include "data_repr.h"
#include "utils/expected.h"

#include "common/proto/storage_client.pb.h"

namespace NDTS::NTabasco {

class TStorageClient {
public:
    enum class TErrorCode {};
    struct TError { std::string msg; };
    using TOptionalError = std::optional<std::string>;

public:
    TStorageClient(const TStorageClientConfig& config);

public:
    TOptionalError UpsertTaskTests(TTask task);

    TOptionalError UpsertTaskMetaData(TTaskMetaData taskMetaData);

    TOptionalError UpsertBuild(TBuild build);

    TExpected<TTaskTests, TError> GetTaskTestsBatch(uint64_t taskId, uint64_t batchId);

    TExpected<TTaskMetaData, TError> GetTaskMetaData(uint64_t taskId);

    TExpected<TBuild, TError> GetBuild(uint64_t buildId);

    TExpected<TBuilds, TError> GetBuilds();

private:
    struct TImpl;

private:
    std::shared_ptr<TImpl> pImpl_;
};

} // end of NDTS::NTabasco namespace
