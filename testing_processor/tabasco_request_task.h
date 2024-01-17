#pragma once

#include "task_batcher_storage_coordinator/grpc/proto/tabasco_grpc.grpc.pb.h"

#include "utils/expected.h"

#include <memory>
#include <string>
#include <vector>

namespace NDTS::NTestingProcessor {

struct TGetScriptsResponse {
    std::string initScript;
    std::string executeScript;
    uint64_t batchCount;
    std::string rootDir;
};

struct TGetBatchResponse {
    std::vector<std::string> inputTests;
    std::vector<std::string> outputTests;
};

struct TErrorResponse {
    std::string msg;
};

class TTabascoRequestTask {
public:
    TTabascoRequestTask(std::shared_ptr<NTabasco::TTabascoGRPC::Stub> tabasco);

    TExpected<TGetScriptsResponse, TErrorResponse> GetScripts(uint64_t taskId, uint64_t buildId);

    TExpected<TGetBatchResponse, TErrorResponse> GetBatch(uint64_t taskId, uint64_t batchId);
private:
    std::shared_ptr<NTabasco::TTabascoGRPC::Stub> tabasco_;
};

} // end of NDTS::NTestingProcessor namespace
