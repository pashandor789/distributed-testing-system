#include "testing_report.h"

namespace NDTS::NTestingProcessor {

std::string GetStringRepr(TVerdict verdict) {
    switch (verdict) {
        case TVerdict::OK:
            return "OK";
        case TVerdict::WA:
            return "WA";
        case TVerdict::TL:
            return "TL";
        case TVerdict::ML:
            return "ML";
        case TVerdict::RE:
            return "RE";
        case TVerdict::CE:
            return "CE";
        case TVerdict::CRASH:
            return "CRASH";
        default:
            return "NOT DEFINED VERDICT ENUM TYPE: " + std::to_string(static_cast<int>(verdict));
    }
}

TTestingReport::TTestingReport(uint64_t cpuTimeElapsedMilliSeconds, uint64_t memorySpent, TVerdict verdict)
    : cpuTimeElapsedMilliSeconds(cpuTimeElapsedMilliSeconds)
    , memorySpent(memorySpent)
    , verdict(verdict)
{}

nlohmann::json NDTS::NTestingProcessor::TTestingReport::ToJSON() const {
    nlohmann::json data;

    data["verdict"] = GetStringRepr(verdict);
    data["cpuTimeElapsedMilliSeconds"] = cpuTimeElapsedMilliSeconds;
    data["memorySpent"] = memorySpent;

    return data;
}

std::vector<nlohmann::json> ToJSON(const std::vector<TTestingReport>& reports) {
    std::vector<nlohmann::json> jsonData;
    jsonData.reserve(reports.size());

    for (auto& report: reports) {
        jsonData.push_back(report.ToJSON());
    }

    return jsonData;
}

} // end of NDTS::NTestingProcessor namespace

