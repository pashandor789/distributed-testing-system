#pragma once

#include <string>
#include <filesystem>

namespace NDTS::NExecutor {

struct TExecutorArgs {
    std::string execute;
    uint64_t cpuTimeLimitSeconds;
    std::filesystem::path outputReportFile;
};

class TExecutor {
public:
    // output as JSON
    void Execute(const TExecutorArgs& executorArgs);
};

} // end of NDTS::Executor namespace
