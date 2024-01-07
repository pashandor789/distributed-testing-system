#pragma once

#include <optional>
#include <string>

#include "testing_report.h"

namespace NDTS::NTestingProcessor {

class TCommitServiceRequest {
public:
    TCommitServiceRequest(std::string url);

    using TCommitError = std::optional<std::string>;

    TCommitError Commit(uint64_t submissionId, const std::vector<TTestingReport>& report);

private:
    std::string url_;
};

} // end of NDTS::NTestingProcessor namespace
