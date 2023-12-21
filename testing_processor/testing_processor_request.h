#pragma once

#include <cstdint>
#include <string>

namespace NDTS::NTestingProcessor {

struct TTestingProcessorRequest {
    template <typename TJSON>
    TTestingProcessorRequest(TJSON json)
        : submissionId(json["submissionId"])
        , buildId(json["buildId"])
        , userData(std::move(json["userData"]))
        , taskId(json["taskId"])
        , memoryLimit(json["memoryLimit"])
        , cpuTimeLimitSeconds(json["cpuTimeLimitSeconds"])
    {}

    uint64_t submissionId;
    uint64_t buildId;
    std::string userData;
    uint64_t taskId;
    uint64_t memoryLimit;
    float cpuTimeLimitSeconds;
};

} // end of NDTS::NTestingProcessor namespace
