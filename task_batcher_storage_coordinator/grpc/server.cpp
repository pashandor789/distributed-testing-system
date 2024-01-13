#include "server.h"

#include <nlohmann/json.hpp>

namespace NDTS::NTabasco {

TTabascoGRPCServiceImpl::TTabascoGRPCServiceImpl(const TTabascoServerConfig& config)
    : storageClient_(config.storage_client_config())
{}

grpc::Status TTabascoGRPCServiceImpl::GetBatch(grpc::ServerContext* context, const TGetBatchRequest* request, TGetBatchResponse* reply) {
    std::string taskId = std::to_string(request->task_id());
    size_t batchId = request->batch_id();

    auto maybeData = storageClient_.GetTaskMeta(taskId);

    if (!maybeData.has_value()) {
        return {grpc::StatusCode::NOT_FOUND, "meta.json wasn't found"};
    }

    auto data = std::move(maybeData.value());

    nlohmann::json metaData = nlohmann::json::parse(std::move(data), nullptr, false);

    auto batches = std::move(metaData["batches"]);

    for (const auto& batchTestInd : batches[batchId]) {
        std::string testIndex = std::to_string(batchTestInd.get<int>());
        auto maybeInputTest = storageClient_.GetTest(taskId, testIndex, "input");
        auto maybeOutputTest = storageClient_.GetTest(taskId, testIndex, "output");

        reply->add_input(std::move(maybeInputTest.value()));
        reply->add_output(std::move(maybeOutputTest.value()));
    }

    return grpc::Status::OK;
}

grpc::Status TTabascoGRPCServiceImpl::GetScripts(grpc::ServerContext* context, const TGetScriptsRequest* request, TGetScriptsResponse* reply) {
    std::string taskId = std::to_string(request->task_id());
    
    auto maybeBuild = storageClient_.GetBuild(request->build_name());

    if (!maybeBuild.has_value()) {
        return {grpc::StatusCode::NOT_FOUND, "Scripts weren't found"};
    }

    TBuild build = std::move(maybeBuild.value()) ;

    if (auto maybeData = storageClient_.GetTaskMeta(taskId)) {
        auto data = std::move(maybeData.value());

        nlohmann::json metaData = nlohmann::json::parse(std::move(data), nullptr, false);

        size_t batchCount = metaData["batches"].size();
        reply->set_batch_count(batchCount);
    }
    
    reply->set_init_script(std::move(build.initScript));
    reply->set_execute_script(std::move(build.executeScript));

    if (auto maybeRootDir = storageClient_.GetTaskRootDir(taskId)) {
        reply->set_task_root_dir(std::move(maybeRootDir.value()));
    }

    return grpc::Status::OK;
}

} // end of NDTS::NTabasco namespace
