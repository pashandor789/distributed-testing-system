#include "create_build_handler.h"

#include "parse_json.h"

#include <vector>

namespace NDTS::NTabasco {

bool TCreateBuildHandler::Parse(const crow::request& req, crow::response& res) {
    auto parseResult = ParseJSON(
        req.body,
        {"buildName", "description", "initScript", "executeScript"}
    );

    if (parseResult.HasError()) {
        res.body = parseResult.Error();
        res.code = 400;
        return false;
    }

    nlohmann::json data = parseResult.Value();

    buildName_ = std::move(data["buildName"]);
    description_ = std::move(data["description"]);
    executeScript_ = data["executeScript"];
    initScript_ = data["initScript"];

    return true;
}

void TCreateBuildHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    if (!Parse(req, res)) {
        return;
    }

    TBuild build{
        .name = std::move(buildName_),
        .description = std::move(description_),
        .initScript = std::move(initScript_),
        .executeScript = std::move(executeScript_)
    };

    bool success =
        ctx.server->storageClient_.CreateBuild(
            std::move(build)
        );

    if (!success) {
        res.code = 500;
        res.body = "build db error!";
        return;
    }
}

} // end of NDTS::TTabasco namespace
