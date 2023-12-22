#pragma once

#include <string>
#include <filesystem>

namespace NDTS::NExecutor {

struct TExecutorArgs {
    std::string executablePath;
    uint64_t cpuTimeLimitSeconds;
};

class TExecutor {
public:
    // output as JSON
    void Execute(const TExecutorArgs& executorArgs, const std::filesystem::path& outputReportFile);
};

} // end of NDTS::Executor namespace
