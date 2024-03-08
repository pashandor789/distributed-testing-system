#include "builds_handler.h"

namespace NDTS::NTabasco {

void TBuildsHandler::Handle(const crow::request &req, crow::response &res, const TContext &ctx) {
    auto expectedBuilds = ctx.server->storageClient_.GetBuilds();

    if (expectedBuilds.HasError()) {
        res.code = expectedBuilds.Error().errorCode;
        res.body = std::move(expectedBuilds.Error().msg);
        return;
    }

    res.body = expectedBuilds.Value().MoveToJSON().dump();
}

} // end of NDTS::TTabasco namespace