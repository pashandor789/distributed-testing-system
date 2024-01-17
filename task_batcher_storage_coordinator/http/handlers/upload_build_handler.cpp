#include "upload_build_handler.h"

#include "utils/parse_json.h"

#include <vector>

namespace NDTS::NTabasco {

bool TUploadBuildHandler::Parse(const crow::request& req, crow::response& res) {
    auto parseResult = ParseJSON(
        req.body,
        {"id", "executeScript", "initScript"}
    );

    if (parseResult.HasError()) {
        res.body = parseResult.Error();
        res.code = 400;
        return false;
    }

    build_ = TBuild::FromJSON(parseResult.Value());

    return true;
}

void TUploadBuildHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    if (!Parse(req, res)) {
        return;
    }

    auto error =
        ctx.server->storageClient_.UpsertBuild(
            std::move(build_)
        );

    if (error.has_value()) {
        res.code = 500;
        res.body = "UpsertBuild Error: " + error.value();
        return;
    }
}

} // end of NDTS::TTabasco namespace
