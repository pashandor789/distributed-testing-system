#pragma once

#include <cstdint>
#include <string>

struct TTestingProcessorRequest {
    template <typename TJSON>
    TTestingProcessorRequest(const TJSON& json)
        : submissionId(json["submissionId"])
        , buildId(json["buildId"])
        , userData(json["userData"])
    {}

    uint64_t submissionId;
    uint64_t buildId;
    std::string userData;
};