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
        , cpuTimeLimitMilliSeconds(json["cpuTimeLimitMilliSeconds"])
    {}

    uint64_t submissionId;
    uint64_t buildId;
    std::string userData;
    uint64_t taskId;
    uint64_t memoryLimit;
    uint64_t cpuTimeLimitMilliSeconds;
};

} // end of NDTS::NTestingProcessor namespace
