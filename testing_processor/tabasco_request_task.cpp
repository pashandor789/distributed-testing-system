#include "tabasco_request_task.h"

namespace NDTS::NTestingProcessor {

TTabascoRequestTask::TTabascoRequestTask(std::shared_ptr<NTabasco::TTabascoGRPC::Stub> tabasco)
    : tabasco_(tabasco)
{}

TGetScriptsResponse TTabascoRequestTask::GetScripts(uint64_t taskId, uint64_t buildId) {
    NTabasco::TGetScriptsRequest request;

    request.set_task_id(taskId);
    request.set_build_id(buildId);

    grpc::ClientContext context;

    NTabasco::TGetScriptsResponse response;
    grpc::Status status = tabasco_->GetScripts(&context, request, &response);

    if (!status.ok()) {
        std::cerr << response.init_script() << std::endl;
        std::cerr << status.error_message() << std::endl;
    }

    return TGetScriptsResponse{
        .initScript = std::move(response.init_script()),
        .executeScript = std::move(response.execute_script()),
        .batchCount = response.batch_count()
    };
}

TGetBatchResponse TTabascoRequestTask::GetBatch(uint64_t taskId, uint64_t batchId) {
    NTabasco::TGetBatchRequest request;

    request.set_task_id(taskId);
    request.set_batch_id(batchId);

    grpc::ClientContext context;

    NTabasco::TGetBatchResponse response;
    grpc::Status status = tabasco_->GetBatch(&context, request, &response);

    std::vector<std::string> inputTests;
    std::vector<std::string> outputTests;

    for (auto& test: response.input()) {
        inputTests.push_back(std::move(test));
    }

    for (auto& test: response.output()) {
        outputTests.push_back(std::move(test));
    }

    return TGetBatchResponse{
        .inputTests = std::move(inputTests),
        .outputTests = std::move(outputTests),
    };
}

} // end of NDTS::NTestingProcessor namespace
