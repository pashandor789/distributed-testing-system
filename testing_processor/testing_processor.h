#pragma once

#include <crow.h>
#include <grpcpp/grpcpp.h>

#include "docker_container.h"
#include "testing_processor_request.h"

#include "proto/testing_processor_config.pb.h"
#include "task_batcher_storage_coordinator/grpc/proto/tabasco_grpc.grpc.pb.h"

namespace NDTS::NTestingProcessor {

struct TTestingReport;

class TTestingProcessor {
public:
    TTestingProcessor(const TTestingProcessorConfig& config);

    void Process(TTestingProcessorRequest request);

private:
    bool Prepare(TTestingProcessorRequest& request);

    std::vector<TTestingReport> Test(TTestingProcessorRequest& request);

    void Commit(TTestingProcessorRequest& request, std::vector<TTestingReport>&& report);

    TTestingReport ExecuteAndTest(
        TTestingProcessorRequest& request,
        const std::string& inputTest,
        const std::string& outputTest
    );

private:
    TDockerContainer container_;
    std::shared_ptr<NTabasco::TTabascoGRPC::Stub> tabasco_;
    std::filesystem::path localStoragePath_;
    std::string commitServiceURL_;
};

} // end of NDTS::NTestingProcessor namespace
