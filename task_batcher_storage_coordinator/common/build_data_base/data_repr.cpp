#include "data_repr.h"

namespace NDTS::NTabasco {

nlohmann::json TInitScript::MoveToJSON() {
    nlohmann::json data;

    data["id"] = id;
    data["name"] = std::move(name);
    data["content"] = std::move(content);

    return data;
}

nlohmann::json TInitScripts::MoveToJSON() {
    nlohmann::json data;

    std::vector<nlohmann::json> movedScripts;
    movedScripts.reserve(scripts.size());

    for (auto& el: scripts) {
        movedScripts.push_back(el.MoveToJSON());
    }

    data["scripts"] = std::move(movedScripts);

    return data;
}

nlohmann::json TExecuteScript::MoveToJSON() {
    nlohmann::json data;

    data["id"] = id;
    data["name"] = std::move(name);
    data["content"] = std::move(content);

    return data;
}

nlohmann::json TExecuteScripts::MoveToJSON() {
    nlohmann::json data;

    std::vector<nlohmann::json> movedScripts;
    movedScripts.reserve(scripts.size());

    for (auto& el: scripts) {
        movedScripts.push_back(el.MoveToJSON());
    }

    data["scripts"] = std::move(movedScripts);

    return data;
}


} // end of NDTS::TTabasco namespace
