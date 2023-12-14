#include "testing_processor.h"

namespace NDTS::NTestingProcessor {

TTestingProcessor::TTestingProcessor(const TTestingProcessorConfig& config)
    : container_(config.dockerContainerConfig())
{}

TTestingProcessor::Process(const TTestingProcessorRequest& request) {
    container_.Run();

    Prepare(request);
    // Build();
    // Test();

    container_.Kill();
}

TTestingProcessor::Prepare(const TTestingProcessorRequest& request) {
    NTabasco::GetScriptsRequest getScriptsRequest;
    getScriptsRequest.set_taskid(request.taskId);

    NTabasco::GetScriptsResponse getScriptsResponse;
    grpc::ClientContext context;

    tabasco_->GetScripts(&context, request, &response);

    
}

} // end of NDTS::NTestingProcessor namespace