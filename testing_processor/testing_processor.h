#pragma once

#include <crow.h>

#include "docker_container.h"
#include "testing_processor_request.h"

#include "testing_processor_config.pb.h"

namespace NDTS::NTestingProcessor {

class TTestingProcessor {
public:
    TTestingProcessor(const TTestingProcessorConfig& config);

    void Process(TTestingProcessorRequest request);

private:
    bool Prepare(TTestingProcessorRequest& request, uint64* const batchCount);

    void Test(TTestingProcessorRequest& request, uint64_t batchCount);

private:
    TDockerContainer container_;
    std::shared_ptr<TTabascoGRPC::Stub> tabasco_;
};

} // end of NDTS::NTestingProcessor namespace
