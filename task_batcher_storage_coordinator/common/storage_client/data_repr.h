#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace NDTS::NTabasco {

struct TTaskTests {
    std::vector<std::string> inputTests{};
    std::vector<std::string> outputTests{};
};

struct TTask {
    uint64_t id = 0;
    TTaskTests tests{};
};

struct TTaskMetaData {
    uint64_t taskId = 0;
    std::vector<std::vector<size_t>> batches{};
    size_t batchSize = 0;

    static TTaskMetaData FromJSON(nlohmann::json data);
    nlohmann::json MoveToJSON();
};

struct TBuild {
    uint64_t id = 0;
    std::string initScript{};
    std::string executeScript{};

    static TBuild FromJSON(nlohmann::json data);
    nlohmann::json MoveToJSON();
};

struct TBuilds {
    std::vector<TBuild> items{};

    static TBuilds FromJSON(nlohmann::json data);
    nlohmann::json MoveToJSON();
};

} // end of NDTS::TTabasco namespace