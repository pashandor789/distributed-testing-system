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

nlohmann::json TTaskMetaData::MoveToJSON() {
    nlohmann::json data;

    data["taskId"] = taskId;
    data["batches"] = std::move(batches);
    data["batchSize"] = batchSize;

    return data;
}

TTaskMetaData TTaskMetaData::FromJSON(nlohmann::json data) {
    TTaskMetaData taskMetaData;

    if (data.contains("batches")) {
        taskMetaData.batches = std::move(data["batches"]);
    }

    if (data.contains("taskId")) {
        taskMetaData.taskId = data["taskId"];
    }

    if (data.contains("batchSize")) {
        taskMetaData.batchSize = data["batchSize"];
    }

    return taskMetaData;
}


nlohmann::json TBuild::MoveToJSON() {
    nlohmann::json data;

    data["id"] = id;
    data["initScript"] = std::move(initScript);
    data["executeScript"] = std::move(executeScript);

    return data;
}

TBuild TBuild::FromJSON(nlohmann::json data) {
    TBuild build;

    if (data.contains("id")) {
        build.id = std::move(data["id"]);
    }

    if (data.contains("initScript")) {
        build.initScript = std::move(data["initScript"]);
    }

    if (data.contains("executeScript")) {
        build.executeScript = std::move(data["executeScript"]);
    }

    return build;
}

nlohmann::json TBuilds::MoveToJSON() {
    nlohmann::json data;
    data["builds"] = MoveToJSONVector(std::move(items));
    return data;
}

TBuilds TBuilds::FromJSON(nlohmann::json data) {
    TBuilds builds;
    builds.items.reserve(data["builds"].size());

    for (auto& item: data["builds"]) {
        builds.items.push_back(TBuild::FromJSON(item));
    }

    return builds;
}

} // end of NDTS::TTabasco namespace
