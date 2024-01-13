#pragma once

#include "nlohmann/json.hpp"

namespace NDTS::NTestingProcessor {

enum class TVerdict {
    OK = 0,
    WA = 1,
    TL = 2,
    ML = 3,
    RE = 4,
    CE = 5,
    CRASH = 6
};

struct TTestingReport {
    TTestingReport(
        uint64_t cpuTimeElapsedMilliSeconds,
        uint64_t memorySpent,
        TVerdict verdict = TVerdict::OK
    );

    nlohmann::json ToJSON() const;

    uint64_t cpuTimeElapsedMilliSeconds;
    uint64_t memorySpent;
    TVerdict verdict;
};

std::vector<nlohmann::json> ToJSON(const std::vector<TTestingReport>& reports);

} // end of NDTS::NTestingProcessor namespace