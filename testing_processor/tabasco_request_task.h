#pragma once

#include <memory>

namespace NDTS::NTestingProcessor {

struct TGetScriptsResponse {
    std::string initScript;
    std::string executeScript;
    uint64_t batchCount;
};

class TTabascoRequestTask {
public:
    TTabascoRequestTask();

    TGetScriptsResponse GetScripts(uint64_t taskId, uint64_t buildId);

    TGetBatchResponse GetBatch(uint64_t taskId, uint64_t batchId);
};

} // end of NDTS::NTestingProcessor namespace