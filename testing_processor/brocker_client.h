#pragma once

#include "proto/brocker_client.pb.h"
#include "proto/testing_processor_config.pb.h"

#include "testing_processor.h"

namespace NDTS::NTestingProcessor {

class TBrockerClient {
public:
    TBrockerClient(const TTestingProcessorConfig& testingProcessorConfig);

    void Run();

private:
    TTestingProcessor testingProcessor_;
private:
    std::string serverURL_;
    std::string queueName_;
};

} // end of NDTS::NTestingProcessor namespace
