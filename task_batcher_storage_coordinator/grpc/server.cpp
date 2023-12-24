#include "server.h"

#include <nlohmann/json.hpp>

namespace NDTS::NTabasco {

TTabascoGRPCServiceImpl::TTabascoGRPCServiceImpl(const TTabascoServerConfig& config)
    : storageClient_(config.storage_client_config())
    , builds_(config.build_data_base_config())
{
}

grpc::Status TTabascoGRPCServiceImpl::GetBatch(grpc::ServerContext* context, const TGetBatchRequest* request, TGetBatchResponse* reply) {
    std::string taskId = std::to_string(request->task_id());
    size_t batchId = request->batch_id();

    auto [getSuccess, data] = storageClient_.GetData(taskId, "meta.json");
    nlohmann::json metaData = nlohmann::json::parse(std::move(data), nullptr, false);

    auto batches = std::move(metaData["batches"]);

    for (size_t i = 0; i < batches[batchId].size(); ++i) {
        auto [inputOk, inputTest] = storageClient_.GetData(taskId, std::to_string(batches[batchId][i].get<int>()) + "_test");
        auto [outputOk, outputTest] = storageClient_.GetData(taskId, std::to_string(batches[batchId][i].get<int>()) + "_answer");

        reply->add_input(std::move(inputTest));
        reply->add_output(std::move(outputTest));
    }

    return grpc::Status::OK;
}

grpc::Status TTabascoGRPCServiceImpl::GetScripts(grpc::ServerContext* context, const TGetScriptsRequest* request, TGetScriptsResponse* reply) {
    std::cerr << "recieved" << std::endl;
    
    std::string taskId = std::to_string(request->task_id());
    
    TScripts scripts = builds_.GetScripts(request->build_id());

    auto [getSuccess, data] = storageClient_.GetData(taskId, "meta.json");
    nlohmann::json metaData = nlohmann::json::parse(std::move(data), nullptr, false);

    size_t batchCount = metaData["batches"].size();

    reply->set_init_script(std::move(scripts.initScript));
    reply->set_execute_script(std::move(scripts.executeScript));
    reply->set_batch_count(batchCount);

    return grpc::Status::OK;
}

} // end of NDTS::NTabasco namespace
