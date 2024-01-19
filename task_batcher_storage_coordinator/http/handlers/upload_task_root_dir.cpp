#include "upload_task_root_dir.h"

#include "utils/parse_json.h"

namespace NDTS::NTabasco {

bool TUploadTaskRootDirHandler::Parse(const crow::request& req, crow::response& res) {
    crow::multipart::message msg(req);

    auto rootDirIt = msg.part_map.find("root_dir.zip");

    if (rootDirIt == msg.part_map.end()) {
        return false;
    }

    auto metaDataIt = msg.part_map.find("data.json");

    if (metaDataIt == msg.part_map.end()) {
        return false;
    }

    auto parseResult = ParseJSON(
        metaDataIt->second.body,
        {"taskId"}
    );

    if (parseResult.HasError()) {
        res.body = parseResult.Error();
        res.code = 400;
        return false;
    }

    nlohmann::json data = std::move(parseResult.Value());

    rootDirZipData_ = std::move(rootDirIt->second.body);
    taskId_ = std::move(data["taskId"]);

    return true;
}

void TUploadTaskRootDirHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    res.code = 501;
    res.body = "Not implemented yet! :)";
}

} // end of NDTS::TTabasco namespace
