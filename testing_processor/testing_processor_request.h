#pragma once

#include <cstdint>
#include <string>

namespace NDTS::NTestingProcessor {

struct TTestingProcessorRequest {
    template <typename TJSON>
    TTestingProcessorRequest(TJSON json)
        : submissionId(json["submissionId"])
        , buildName(json["buildName"])
        , userData(std::move(json["userData"]))
        , taskId(json["taskId"])
        , memoryLimit(json["memoryLimit"])
        , cpuTimeLimitMilliSeconds(json["cpuTimeLimitMilliSeconds"])
        , batchCount(0)
    {}

    uint64_t submissionId;
    std::string buildName;
    std::string userData;
    uint64_t taskId;
    uint64_t memoryLimit;
    uint64_t cpuTimeLimitMilliSeconds;
    uint64_t batchCount; // specified in TTestingProcessor::Prepare
};

} // end of NDTS::NTestingProcessor namespace
