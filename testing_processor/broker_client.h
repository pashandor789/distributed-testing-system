#pragma once

#include "proto/broker_client.pb.h"
#include "proto/testing_processor_config.pb.h"

#include "testing_processor.h"

namespace NDTS::NTestingProcessor {

class TBrokerClient {
public:
    TBrokerClient(const TTestingProcessorConfig& testingProcessorConfig);

    void Run();

private:
    TTestingProcessor testingProcessor_;
private:
    std::string serverURL_;
    std::string queueName_;
};

} // end of NDTS::NTestingProcessor namespace
