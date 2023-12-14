#pragma once

#include <cstdint>
#include <string>

struct TTestingProcessorRequest {
    template <typename TJSON>
    TTestingProcessorRequest(const TJSON& json)
        : submissionId(json["submissionId"])
        , buildId(json["buildId"])
        , userData(json["userData"])
        , taskId(json["taskId"])
    {}

    uint64_t submissionId;
    uint64_t buildId;
    std::string userData;
    uint64_t taskId;
};