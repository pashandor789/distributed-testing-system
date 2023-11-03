#pragma once

#include <filesystem>
#include <string>
#include <optional>
#include <vector>

namespace NDTS::NTestingProcessor {

namespace fs = std::filesystem;

class TDockerContainer {
public:
    TDockerContainer(const std::string& image)
        : image_(image)
    {
    }

    void Run();

    void Exec(
        std::vector<std::string> scriptArgs, 
        const std::optional<fs::path>& stdIn,
        const std::optional<fs::path>& stdOut
    );

    ~TDockerContainer();

private:
    void Stop();

    void Remove();

private:
    std::string containerId_;
    std::string image_;
};

} // end of NDTS::NTestingProcessor namespace
