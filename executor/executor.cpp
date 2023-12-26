#include "executor.h"

#include "utils/execv_args.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

namespace NDTS::NExecutor {

int pidFdOpen(pid_t pid) {
    return static_cast<int>(syscall(__NR_pidfd_open, pid, 0));
}

void TExecutor::Execute(const TExecutorArgs& executorArgs) {
    TExecVArgs execVArgs("/usr/bin/sh", {"/usr/bin/sh", "-c", executorArgs.execute});

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

        perror("error: ");
        exit(1);
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

    kill(-pid, SIGKILL);

    close(epollFd);
    close(childPidFd);

    int waitStatus = 0;
    rusage processInfo{};
    wait4(pid, &waitStatus, 0, &processInfo);

    int exitCode = !WIFEXITED(waitStatus) || WEXITSTATUS(waitStatus);

    uint64_t cpuTimeElapsedMicroSeconds = 
        1'000'000 * processInfo.ru_stime.tv_sec + processInfo.ru_stime.tv_usec +
        1'000'000 * processInfo.ru_utime.tv_sec + processInfo.ru_utime.tv_usec;
    
    nlohmann::json report;

    report["cpuTimeElapsedMicroSeconds"] = cpuTimeElapsedMicroSeconds;
    report["wallTimeElapsedMicroSeconds"] = wallTimeElapsedMicroSeconds;
    report["memorySpent"] = static_cast<uint64_t>(processInfo.ru_maxrss);
    report["exitCode"] = exitCode;

    std::ofstream output(executorArgs.outputReportFile);

    output << report.dump();
    output.flush();
}

} // end of NDTS::Executor namespace
