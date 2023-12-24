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
    bool Prepare(TTestingProcessorRequest& request, uint64_t* const batchCount);

    std::vector<TTestingReport> Test(TTestingProcessorRequest& request, uint64_t batchCount);

    void Commit(std::vector<TTestingReport>&& report);

private:
    TDockerContainer container_;
    std::shared_ptr<NTabasco::TTabascoGRPC::Stub> tabasco_;
    std::filesystem::path localStoragePath_;
};

} // end of NDTS::NTestingProcessor namespace
