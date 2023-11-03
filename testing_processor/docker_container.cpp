#include "docker_container.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace NDTS::NTestingProcessor {

class TExecVArgs {
public:
    TExecVArgs(std::string&& pathName, std::vector<std::string>&& args)
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

TExecVArgs GetExecCommandArgs(const std::string& containerId, std::vector<std::string>&& scriptArgs) {
    std::vector<std::string> dockerCommand = {"/usr/bin/docker", "exec", containerId};
    scriptArgs.insert(scriptArgs.begin(), dockerCommand.begin(), dockerCommand.end());

    return {"/usr/bin/docker", std::move(scriptArgs)};
}

void TDockerContainer::Run() {
    std::string command = "/usr/bin/docker run -itd";

    command
        .append(" ")
        .append(image_);

    FILE* dockerOut = popen(command.c_str(), "r");

    for (auto ch = fgetc(dockerOut); ch != EOF && ch != '\n'; ch = fgetc(dockerOut)) {
        containerId_.push_back(ch);
    }

    fclose(dockerOut);
}

void TDockerContainer::Exec(
    std::vector<std::string> scriptArgs, 
    const std::optional<fs::path>& stdIn,
    const std::optional<fs::path>& stdOut
) {
    auto command = GetExecCommandArgs(containerId_, std::move(scriptArgs));

    pid_t pid = fork();

    if (pid == 0) {
        if (stdIn.has_value()) {
            int fd = open(stdIn.value().c_str(), 0, O_RDONLY);
            dup2(fd, STDIN_FILENO);
        }

        if (stdOut.has_value()) {
            int fd = open(stdOut.value().c_str(), 0, O_WRONLY);
            dup2(fd, STDOUT_FILENO);
        }

        execv(command.GetPathName(), command.GetArgV());

        perror("docker container exec fail: ");
        exit(1);
    }

    waitpid(pid, nullptr, 0); 
}

void TDockerContainer::Stop() {
    std::string command = "/usr/bin/docker stop";

    command
        .append(" ")
        .append(containerId_);

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
    Stop();
    Remove();
}

} // end of NDTS::NTestingProcessor namespace
