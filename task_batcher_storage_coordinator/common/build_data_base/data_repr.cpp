#include "data_repr.h"

namespace NDTS::NTabasco {

template <typename TItems>
std::vector<nlohmann::json> MoveToJSONVector(TItems items) {
    std::vector<nlohmann::json> movedData;

    movedData.reserve(items.size());

    for (auto& item: items) {
        movedData.push_back(item.MoveToJSON());
    }

    return movedData;
}

nlohmann::json TInitScript::MoveToJSON() {
    nlohmann::json data;

    data["id"] = id;
    data["name"] = std::move(name);
    data["content"] = std::move(content);

    return data;
}

nlohmann::json TInitScripts::MoveToJSON() {
    nlohmann::json data;
    data["items"] = MoveToJSONVector(std::move(items));
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
    data["items"] = MoveToJSONVector(std::move(items));
    return data;
}

nlohmann::json TBuild::MoveToJSON() {
    nlohmann::json data;

    data["id"] = id;
    data["name"] = std::move(name);

    data["initScript"] = initScript.MoveToJSON();
    data["executeScript"] = executeScript.MoveToJSON();

    return data;
}

nlohmann::json TBuilds::MoveToJSON() {
    nlohmann::json data;
    data["items"] = MoveToJSONVector(std::move(items));
    return data;
}

} // end of NDTS::TTabasco namespace
