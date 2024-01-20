#include "build_handler.h"

#include "utils/parse_json.h"

namespace NDTS::NTabasco {

bool TBuildHandler::Parse(const crow::request& req, crow::response& res) {
    const char* serializedId = req.url_params.get("id");

    if (serializedId == nullptr) {
        res.code = 400;
        res.body = "id wasn't specified in url!";
        return false;
    }

    buildId_ = stoull(std::string(serializedId));

    return true;
}


void TBuildHandler::Handle(const crow::request &req, crow::response &res, const TContext &ctx) {
    if (!Parse(req, res)) {
        return;
    }

    auto expectedBuild = ctx.server->storageClient_.GetBuild(buildId_);

    if (expectedBuild.HasError()) {
        res.code = 500;
        res.body = "GetBuild Error: " + expectedBuild.Error().msg;
        return;
    }

    res.body = expectedBuild.Value().MoveToJSON().dump();
}

} // end of NDTS::TTabasco namespace