#include "execute_scripts_handler.h"
#include "nlohmann/json.hpp"

namespace NDTS::NTabasco {

void TExecuteScriptsHandler::Handle(const crow::request& req, crow::response& res, const TContext& ctx) {
    TExecuteScripts scripts = ctx.server->builds_.GetExecuteScripts();
    res.body = scripts.MoveToJSON().dump();
}

} // end of NDTS::TTabasco namespace