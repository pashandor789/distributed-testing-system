#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace NDTS::NTabasco {

struct TScripts {
    std::string initScript;
    std::string executeScript;
};

struct TInitScript {
    uint64_t id;
    std::string name;
    std::string content;

    nlohmann::json MoveToJSON();
};

struct TInitScripts {
    std::vector<TInitScript> items;

    nlohmann::json MoveToJSON();
};

struct TExecuteScript {
    uint64_t id;
    std::string name;
    std::string content;

    nlohmann::json MoveToJSON();
};

struct TExecuteScripts {
    std::vector<TExecuteScript> items;

    nlohmann::json MoveToJSON();
};

struct TBuild {
    uint64_t id;
    std::string name;

    TInitScript initScript;
    TExecuteScript executeScript;

    nlohmann::json MoveToJSON();
};

struct TBuilds {
    std::vector<TBuild> items;

    nlohmann::json MoveToJSON();
};

} // end of NDTS::TTabasco namespace