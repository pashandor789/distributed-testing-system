#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace NDTS::NTabasco {

struct TBuild {
    std::string name{};
    std::string description{};
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