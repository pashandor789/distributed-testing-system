#include "server.h"

#include <nlohmann/json.hpp>

namespace NDTS::NTabasco {

TTabascoGRPCServiceImpl::TTabascoGRPCServiceImpl(const TTabascoServerConfig& config)
    : storageClient_(config.storage_client_config())
{}

grpc::Status TTabascoGRPCServiceImpl::GetBatch(grpc::ServerContext* context, const TGetBatchRequest* request, TGetBatchResponse* reply) {
    uint64_t taskId = request->task_id();
    size_t batchId = request->batch_id();

    auto expectedTaskTestsBatch = storageClient_.GetTaskTestsBatch(taskId, batchId);

    if (expectedTaskTestsBatch.HasError()) {
        return {grpc::StatusCode::NOT_FOUND, expectedTaskTestsBatch.Error().msg};
    }

    auto taskTestsBatch = std::move(expectedTaskTestsBatch.Value());

    *reply->mutable_input() = {
        std::make_move_iterator(taskTestsBatch.inputTests.begin()),
        std::make_move_iterator(taskTestsBatch.inputTests.end())
    };

    *reply->mutable_output() = {
        std::make_move_iterator(taskTestsBatch.outputTests.begin()),
        std::make_move_iterator(taskTestsBatch.outputTests.end())
    };

    return grpc::Status::OK;
}

grpc::Status TTabascoGRPCServiceImpl::GetScripts(grpc::ServerContext* context, const TGetScriptsRequest* request, TGetScriptsResponse* reply) {
    uint64_t taskId = request->task_id();
    
    auto expectedBuild = storageClient_.GetBuild(request->build_id());

    if (expectedBuild.HasError()) {
        return {grpc::StatusCode::NOT_FOUND, expectedBuild.Error().msg};
    }

    TBuild build = std::move(expectedBuild.Value());

    reply->set_init_script(std::move(build.initScript));
    reply->set_execute_script(std::move(build.executeScript));

    auto expectedTaskMetaData = storageClient_.GetTaskMetaData(taskId);

    if (expectedTaskMetaData.HasError()) {
        return {grpc::StatusCode::NOT_FOUND, expectedTaskMetaData.Error().msg};
    }

    TTaskMetaData taskMetaData = std::move(expectedTaskMetaData.Value());

    reply->set_batch_count(taskMetaData.batches.size());

    return grpc::Status::OK;
}

} // end of NDTS::NTabasco namespace
