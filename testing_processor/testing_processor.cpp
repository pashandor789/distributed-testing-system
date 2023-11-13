#include "testing_processor.h"

namespace NDTS::NTestingProcessor {

TTestingProcessor::TTestingProcessor(const TTestingProcessorConfig& config)
    : container_(config.dockerContainerConfig())
{}

TTestingProcessor::Process(const TTestingProcessorRequest& request) {
    container_.Run();

    Prepare();
    Build();
    Test();

    container_.Kill();
}

} // end of NDTS::NTestingProcessor namespace