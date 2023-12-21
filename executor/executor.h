#pragma once

#include <string>

namespace NDTS::NExecutor {

struct TExecutorArgs {
    std::string executablePath;
    uint64_t cpuTimeLimitSeconds;
};

struct TExecutorReport {
    uint64_t cpuTimeElapsedMicroSeconds;
    uint64_t wallTimeElapsedMicroSeconds;
    uint64_t memorySpent;
    uint64_t exitCode;
};

class TExecutor {
public:
    TExecutorReport Execute(const TExecutorArgs& executorArgs);
};

} // end of NDTS::Executor namespace
