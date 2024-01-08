#include "server.h"

#include <nlohmann/json.hpp>

namespace NDTS::NTabasco {

TTabascoGRPCServiceImpl::TTabascoGRPCServiceImpl(const TTabascoServerConfig& config)
    : storageClient_(config.storage_client_config())
    , builds_(config.build_data_base_config())
{}

grpc::Status TTabascoGRPCServiceImpl::GetBatch(grpc::ServerContext* context, const TGetBatchRequest* request, TGetBatchResponse* reply) {
    std::string taskId = std::to_string(request->task_id());
    size_t batchId = request->batch_id();

    auto [getSuccess, data] = storageClient_.GetData(taskId, "meta.json");

    if (!getSuccess) {
        return {grpc::StatusCode::NOT_FOUND, "meta.json wasn't found"};
    }

    nlohmann::json metaData = nlohmann::json::parse(std::move(data), nullptr, false);

    auto batches = std::move(metaData["batches"]);

    for (const auto& batchTestInd : batches[batchId]) {
        auto [inputOk, inputTest] = storageClient_.GetData(taskId, std::to_string(batchTestInd.get<int>()) + "_input");
        auto [outputOk, outputTest] = storageClient_.GetData(taskId, std::to_string(batchTestInd.get<int>()) + "_output");

        reply->add_input(std::move(inputTest));
        reply->add_output(std::move(outputTest));
    }

    return grpc::Status::OK;
}

grpc::Status TTabascoGRPCServiceImpl::GetScripts(grpc::ServerContext* context, const TGetScriptsRequest* request, TGetScriptsResponse* reply) {
    std::string taskId = std::to_string(request->task_id());
    
    auto maybeScripts = builds_.GetScripts(request->build_id());

    if (!maybeScripts.has_value()) {
        return {grpc::StatusCode::NOT_FOUND, "Scripts weren't found"};
    }

    TScripts scripts = std::move(maybeScripts.value()) ;

    auto [getSuccess, data] = storageClient_.GetData(taskId, "meta.json");

    if (!getSuccess) {
        return {grpc::StatusCode::NOT_FOUND, "meta.json wasn't found"};
    }

    nlohmann::json metaData = nlohmann::json::parse(std::move(data), nullptr, false);

    size_t batchCount = metaData["batches"].size();
    reply->set_batch_count(batchCount);
    
    reply->set_init_script(std::move(scripts.initScript));
    reply->set_execute_script(std::move(scripts.executeScript));

    return grpc::Status::OK;
}

} // end of NDTS::NTabasco namespace
