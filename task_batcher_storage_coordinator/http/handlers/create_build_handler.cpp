#include "create_build_handler.h"

#include "parse_json.h"

#include <vector>

namespace NDTS::NTabasco {

bool TCreateBuildHandler::Parse(const crow::request& req, crow::response& res) {
    auto parseResult = ParseJSON(
        req.body,
        {"buildName", "executeScriptId", "initScriptId"}
    );

    if (parseResult.HasError()) {
        res.body = parseResult.Error();
        res.code = 400;
        return false;
    }

    nlohmann::json data = parseResult.Value();

    buildName_ = std::move(data["buildName"]);
    executeScriptId_ = data["executeScriptId"];
    initScriptId_ = data["initScriptId"];

    return true;
}

void TCreateBuildHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    if (!Parse(req, res)) {
        return;
    }

    bool success =
        ctx.server->builds_.CreateBuild(
                std::move(buildName_),
                executeScriptId_,
                initScriptId_
        );

    if (!success) {
        res.code = 500;
        res.body = "build db error!";
        return;
    }
}

} // end of NDTS::TTabasco namespace
