#pragma once

#include <cstdint>
#include <string>

namespace NDTS::NTestingProcessor {

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

} // end of NDTS::NTestingProcessor namespace
