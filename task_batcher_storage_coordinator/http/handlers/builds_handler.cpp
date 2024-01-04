#include "builds_handler.h"

namespace NDTS::NTabasco {

void TBuildsHandler::Handle(const crow::request &req, crow::response &res, const TContext &ctx) {
    TBuilds builds = ctx.server->builds_.GetBuilds();
    res.body = builds.MoveToJSON().dump();
}

} // end of NDTS::TTabasco namespace