#include "init_scripts_handler.h"
#include "nlohmann/json.hpp"

namespace NDTS::NTabasco {

void TInitScriptsHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    TInitScripts scripts = ctx.server->builds_.GetInitScripts();
    res.body = scripts.MoveToJSON().dump();
}

} // end of NDTS::TTabasco namespace