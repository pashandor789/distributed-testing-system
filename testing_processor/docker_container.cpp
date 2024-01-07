#include "docker_container.h"

#include "utils/execv_args.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace NDTS::NTestingProcessor {

TExecVArgs GetExecCommandArgs(
    const std::string& containerId,
    std::vector<std::string> scriptArgs,
    const std::optional<fs::path>& workingDir
) {
    std::vector<std::string> dockerCommand = {"/usr/bin/docker", "exec", "-i"};

    if (workingDir.has_value()) {
        dockerCommand.push_back("-w");
        dockerCommand.push_back(workingDir.value());
    }

    dockerCommand.push_back(containerId);

    scriptArgs.insert(scriptArgs.begin(), dockerCommand.begin(), dockerCommand.end());

    return {"/usr/bin/docker", std::move(scriptArgs)};
}

TDockerContainer::TDockerContainer(const TDockerContainerConfig& config) 
    : image_(config.image_name())
    , cpuCount_(config.cpu_count())
    , memoryLimit_(config.memory_limit())
    , memorySwapLimit_(config.memory_swap_limit())
    , pidLimit_(config.pid_limit())
{}

void TDockerContainer::Run() {
    std::string command = "/usr/bin/docker run -itd";

    command
        .append(" --cpus ")
        .append(std::to_string(cpuCount_))
        .append(" --memory ")
        .append(memoryLimit_)
        .append(" --memory-swap ")
        .append(memorySwapLimit_)
        .append(" --pids-limit ")
        .append(std::to_string(pidLimit_))
        .append(" ")
        .append(image_);

    FILE* dockerOut = popen(command.c_str(), "r");

    std::string containerId;
    for (auto ch = fgetc(dockerOut); ch != EOF && ch != '\n'; ch = fgetc(dockerOut)) {
        containerId.push_back(ch);
    }

    containerId_ = std::move(containerId);

    fclose(dockerOut);
}

int TDockerContainer::Exec(
    std::vector<std::string> scriptArgs, 
    TDockerExecOptions options
) {
    auto command = GetExecCommandArgs(containerId_, std::move(scriptArgs), options.workingDir);

    pid_t pid = fork();

    if (pid == 0) {
        if (options.stdIn.has_value()) {
            int fd = open(options.stdIn.value().c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO);
        }

        if (options.stdOut.has_value()) {
            int fd = open(options.stdOut.value().c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            dup2(fd, STDOUT_FILENO);
        }

        execv(command.GetPathName(), command.GetArgV());

        perror("docker container exec fail: ");
        exit(1);
    }

    int stat = 0;
    waitpid(pid, &stat, 0);
    int exitCode = !WIFEXITED(stat) || WEXITSTATUS(stat);
    return exitCode; 
}

int TDockerContainer::ExecBash(
    std::vector<std::string> scriptArgs, 
    TDockerExecOptions options
) {
    scriptArgs.insert(scriptArgs.begin(), "/usr/bin/bash");
    return Exec(std::move(scriptArgs), std::move(options));
}

void TDockerContainer::Kill() {
//    std::string command = "/usr/bin/docker stop -s SIGKILL";
//
//    command
//        .append(" ")
//        .append(containerId_);
//
//    std::system(command.c_str());
}

void TDockerContainer::MoveFileInside(const fs::path& outsidePath, const fs::path& containerPath) {
    std::string command = "/usr/bin/docker cp";

    command
        .append(" ")
        .append(outsidePath)
        .append(" ")
        .append(containerId_)
        .append(":")
        .append(containerPath);

    std::system(command.c_str());
}

void TDockerContainer::Remove() {
//    std::string command = "/usr/bin/docker rm";
//
//    command
//        .append(" ")
//        .append(containerId_);
//
//    std::system(command.c_str());
}

void TDockerContainer::CreateFile(const fs::path& path, std::string content) {
    std::string echoCommand;

    echoCommand
        .append("/usr/bin/echo")
        .append(" ")
        .append("'")
        .append(content)
        .append("'")
        .append(" > ")
        .append(path);

    Exec({"sh", "-c", std::move(echoCommand)});
}

TDockerContainer::~TDockerContainer() {
    if (!containerId_.empty()) {
        Kill();
        Remove();
    }
}

} // end of NDTS::NTestingProcessor namespace
