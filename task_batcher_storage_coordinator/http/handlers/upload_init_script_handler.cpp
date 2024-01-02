#include "upload_init_script_handler.h"

#include "utils/utils.h"

#include <vector>

namespace NDTS::NTabasco {

bool TUploadInitScriptHandler::Parse(const crow::request& req, crow::response& res) {
    nlohmann::json data = nlohmann::json::parse(req.body, nullptr, false);

    if (data.is_discarded()) {
        res.code = 400;
        res.body = "invalid json";
        return false;
    }

    auto missingFields = FindMissingJSONFields({"scriptName", "content"}, data);

    if (!missingFields.empty()) {
        res.code = 400;
        res.body = "field's weren't specified: " + Join(std::move(missingFields), ",");
        return false;
    }

    scriptName_ = std::move(data["scriptName"]);
    content_ = std::move(data["content"]);

    return true;
}

void TUploadInitScriptHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    if (!Parse(req, res)) {
        return;
    }

    bool success = ctx.server->builds_.UploadInitScript(
            std::move(scriptName_),
            std::move(content_)
    );

    if (!success) {
        res.code = 500;
        res.body = "build db error!";
        return;
    }
}

} // end of NDTS::TTabasco namespace
