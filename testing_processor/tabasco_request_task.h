#pragma once

#include <memory>
#include <string>
#include <vector>

namespace NDTS::NTestingProcessor {

struct TGetScriptsResponse {
    std::string initScript;
    std::string executeScript;
    uint64_t batchCount;
};

struct TGetBatchResponse {
    std::vector<std::string> inputTests;
    std::vector<std::string> outputTests;
};

class TTabascoRequestTask {
public:
    TTabascoRequestTask();

    TGetScriptsResponse GetScripts(uint64_t taskId, uint64_t buildId);

    TGetBatchResponse GetBatch(uint64_t taskId, uint64_t batchId);
};

} // end of NDTS::NTestingProcessor namespace