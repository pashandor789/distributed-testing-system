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

nlohmann::json TBuild::MoveToJSON() {
    nlohmann::json data;

    data["description"] = std::move(description);
    data["name"] = std::move(name);

    data["initScript"] = std::move(initScript);
    data["executeScript"] = std::move(executeScript);

    return data;
}

TBuild TBuild::FromJSON(nlohmann::json data) {
    TBuild build;

    build.description = std::move(data["description"]);
    build.name = std::move(data["name"]);
    build.initScript = std::move(data["initScript"]);
    build.executeScript = std::move(data["executeScript"]);

    return build;
}

nlohmann::json TBuilds::MoveToJSON() {
    nlohmann::json data;
    data["items"] = MoveToJSONVector(std::move(items));
    return data;
}

TBuilds TBuilds::FromJSON(nlohmann::json data) {
    TBuilds builds;
    builds.items.reserve(data["items"].size());

    for (auto& item: data["items"]) {
        builds.items.push_back(TBuild::FromJSON(item));
    }

    return builds;
}

} // end of NDTS::TTabasco namespace
