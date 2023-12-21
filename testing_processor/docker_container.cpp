#include "docker_container.h"

#include "utils/execv_args.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace NDTS::NTestingProcessor {

TExecVArgs GetExecCommandArgs(const std::string& containerId, std::vector<std::string>&& scriptArgs) {
    std::vector<std::string> dockerCommand = {"/usr/bin/docker", "exec", "-i", containerId};
    scriptArgs.insert(scriptArgs.begin(), dockerCommand.begin(), dockerCommand.end());

    return {"/usr/bin/docker", std::move(scriptArgs)};
}

TDockerContainer::TDockerContainer(const TDockerContainerConfig& config) 
    : image_(config.imagename())
    , cpuCount_(config.cpucount())
    , memoryLimit_(config.memorylimit())
    , memorySwapLimit_(config.memoryswaplimit())
    , pidLimit_(config.pidlimit())
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

    for (auto ch = fgetc(dockerOut); ch != EOF && ch != '\n'; ch = fgetc(dockerOut)) {
        containerId_.push_back(ch);
    }

    fclose(dockerOut);
}

int TDockerContainer::Exec(
    std::vector<std::string> scriptArgs, 
    const std::optional<fs::path>& stdIn,
    const std::optional<fs::path>& stdOut
) {
    auto command = GetExecCommandArgs(containerId_, std::move(scriptArgs));

    pid_t pid = fork();

    if (pid == 0) {
        if (stdIn.has_value()) {
            int fd = open(stdIn.value().c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO);
        }

        if (stdOut.has_value()) {
            int fd = open(stdOut.value().c_str(), O_WRONLY | O_CREAT, S_IRWXU);
            dup2(fd, STDOUT_FILENO);
        }

        execv(command.GetPathName(), command.GetArgV());

        perror("docker container exec fail: ");
        exit(1);
    }

    int stat = 0;
    waitpid(pid, &stat, 0);
    int exitCode = !WEXITED(stat) || WEXITSTATUS(stat);
    return exitCode; 
}

void TDockerContainer::Kill() {
    std::string command = "/usr/bin/docker stop -s SIGKILL";

    command
        .append(" ")
        .append(containerId_);

    std::system(command.c_str());
}

void TDockerContainer::MoveFileInside(const fs::path& outsidePath, const fs::path& containerPath) {
    std::string command = "/usr/bin/docker cp";

    command
        .append(" ")
        .append(outsidePath)
        .append(" ")
        .append(std::to_string(containerId_))
        .append(":")
        .append(containerPath);

    std::system(command.c_str());
}

void TDockerContainer::Remove() {
    std::string command = "/usr/bin/docker rm";

    command
        .append(" ")
        .append(containerId_);

    std::system(command.c_str());  
}

TDockerContainer::~TDockerContainer() {
    Kill();
    Remove();
}

} // end of NDTS::NTestingProcessor namespace
