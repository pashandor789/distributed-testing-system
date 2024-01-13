#include "tabasco_request_task.h"

#include <utility>

namespace NDTS::NTestingProcessor {

TTabascoRequestTask::TTabascoRequestTask(std::shared_ptr<NTabasco::TTabascoGRPC::Stub> tabasco)
    : tabasco_(std::move(tabasco))
{}

TExpected<TGetScriptsResponse, TErrorResponse> TTabascoRequestTask::GetScripts(uint64_t taskId, std::string buildName) {
    NTabasco::TGetScriptsRequest request;

    request.set_task_id(taskId);
    request.set_build_name(buildName);

    grpc::ClientContext context;

    NTabasco::TGetScriptsResponse response;
    grpc::Status status = tabasco_->GetScripts(&context, request, &response);

    if (!status.ok()) {
        return TUnexpected<TErrorResponse>({ .msg = status.error_message() });
    }

    return TGetScriptsResponse{
        .initScript = std::move(*response.mutable_init_script()),
        .executeScript = std::move(*response.mutable_execute_script()),
        .batchCount = response.batch_count(),
        .rootDir = std::move(*response.mutable_task_root_dir())
    };
}

TExpected<TGetBatchResponse, TErrorResponse> TTabascoRequestTask::GetBatch(uint64_t taskId, uint64_t batchId) {
    NTabasco::TGetBatchRequest request;

    request.set_task_id(taskId);
    request.set_batch_id(batchId);

    grpc::ClientContext context;

    NTabasco::TGetBatchResponse response;
    grpc::Status status = tabasco_->GetBatch(&context, request, &response);

    if (!status.ok()) {
        return TUnexpected<TErrorResponse>({ .msg = status.error_message() });
    }

    std::vector<std::string> inputTests;
    std::vector<std::string> outputTests;

    for (auto& test: *response.mutable_input()) {
        inputTests.push_back(std::move(test));
    }

    for (auto& test: *response.mutable_output()) {
        outputTests.push_back(std::move(test));
    }

    return TGetBatchResponse{
        .inputTests = std::move(inputTests),
        .outputTests = std::move(outputTests),
    };
}

} // end of NDTS::NTestingProcessor namespace
