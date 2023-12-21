#include "executor.h"

#include <iostream>
#include <chrono>
#include <string>
#include <vector>

#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

namespace NDTS::NExecutor {

class TExecVArgs {
public:
    TExecVArgs(std::string pathName, std::vector<std::string> args)
        : pathName_(std::move(pathName)) 
        , args_(std::move(args))
    {
        argV_.reserve(args_.size() + 1);

        for (auto& arg: args_) {
            arg.push_back('\0'); //  data is not null-terminated before C++20
            argV_.push_back(arg.data());
        }

        argV_.push_back(nullptr);
    }

    const char* GetPathName() {
        return pathName_.c_str();
    }

    char* const* GetArgV() {
        return const_cast<char* const*>(argV_.data());
    }

private:
    std::string pathName_;
    std::vector<std::string> args_;
private:
    std::vector<char*> argV_;
};

int pidFdOpen(pid_t pid) {
    return static_cast<int>(syscall(__NR_pidfd_open, pid, 0));
}

TExecutorReport TExecutor::Execute(const TExecutorArgs& executorArgs) {
    TExecVArgs execVArgs(executorArgs.executablePath, {executorArgs.executablePath});

    pid_t pid = fork();

    if (pid == 0) {
        if (setpgrp() < 0) {
            exit(1);
        }

        rlimit limit{
            .rlim_cur = static_cast<rlim_t>(executorArgs.cpuTimeLimitSeconds),
            .rlim_max = static_cast<rlim_t>(executorArgs.cpuTimeLimitSeconds)
        };
        setrlimit(RLIMIT_CPU, &limit);

        execv(execVArgs.GetPathName(), execVArgs.GetArgV());
        std::cerr << "bad" << std::endl;
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int epollFd = epoll_create1(0);
    
    int childPidFd = pidFdOpen(pid);
    epoll_event childEpollEvent = {.events = EPOLLIN, .data = {.fd = childPidFd}};
    epoll_ctl(epollFd, EPOLL_CTL_ADD, childPidFd, &childEpollEvent);

    epoll_event passedEvent{0};
    int eventPassed = epoll_wait(epollFd, &passedEvent, 1, static_cast<int>(executorArgs.cpuTimeLimitSeconds * 1000));

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    uint64_t wallTimeElapsedMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "killing " << pid << std::endl;
    std::cout << kill(-pid, SIGKILL) << std::endl;

    close(epollFd);
    close(childPidFd);

    int waitStatus = 0;
    rusage processInfo{};
    wait4(pid, &waitStatus, 0, &processInfo);

    uint64_t cpuTimeElapsedMicroSeconds = 
        1'000'000 * processInfo.ru_stime.tv_sec + processInfo.ru_stime.tv_usec +
        1'000'000 * processInfo.ru_utime.tv_sec + processInfo.ru_utime.tv_usec;

    std::cout << processInfo.ru_isrss << ' ' << processInfo.ru_maxrss << std::endl;
    
    return TExecutorReport{
        .cpuTimeElapsedMicroSeconds = cpuTimeElapsedMicroSeconds,
        .wallTimeElapsedMicroSeconds = wallTimeElapsedMicroSeconds,
        .memorySpent = static_cast<uint64_t>(processInfo.ru_maxrss)
    };
}

} // end of NDTS::Executor namespace