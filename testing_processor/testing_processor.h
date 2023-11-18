#pragma once

#include <crow.h>

#include "docker_container.h"
#include "testing_processor_request.h"

#include "testing_processor_config.pb.h"

namespace NDTS::NTestingProcessor {

class TTestingProcessor {
public:
    TTestingProcessor(const TTestingProcessorConfig& config);

    void Process(const TTestingProcessorRequest& request);

private:
    TDockerContainer container_;
};

} // end of NDTS::NTestingProcessor namespace
