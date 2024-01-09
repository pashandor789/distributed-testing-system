#include "upload_execute_script_handler.h"

#include "utils/parse_json.h"

#include <vector>

namespace NDTS::NTabasco {

bool TUpdateExecuteScriptHandler::Parse(const crow::request& req, crow::response& res) {
    auto parseResult = ParseJSON(req.body, {"buildName", "content"});

    if (parseResult.HasError()) {
        res.body = parseResult.Error();
        res.code = 400;
        return false;
    }

    nlohmann::json data = parseResult.Value();

    buildName_ = std::move(data["buildName"]);
    content_ = std::move(data["content"]);

    return true;
}

void TUpdateExecuteScriptHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    if (!Parse(req, res)) {
        return;
    }

    bool success =
        ctx.server->storageClient_.UploadExecuteScript(
            std::move(buildName_),
            std::move(content_)
        );

    if (!success) {
        res.code = 500;
        res.body = "build db error!";
        return;
    }
}

} // end of NDTS::TTabasco namespace
