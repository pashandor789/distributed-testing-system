#include "upload_execute_script_handler.h"

#include "parse_json.h"

#include <vector>

namespace NDTS::NTabasco {

bool TUploadExecuteScriptHandler::Parse(const crow::request& req, crow::response& res) {
    auto parseResult = ParseJSON(req.body, {"scriptName", "content"});

    if (parseResult.HasError()) {
        res.body = parseResult.Error();
        res.code = 400;
        return false;
    }

    nlohmann::json data = parseResult.Value();

    scriptName_ = std::move(data["scriptName"]);
    content_ = std::move(data["content"]);

    return true;
}

void TUploadExecuteScriptHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    if (!Parse(req, res)) {
        return;
    }

    bool success =
        ctx.server->builds_.UploadExecuteScript(
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
