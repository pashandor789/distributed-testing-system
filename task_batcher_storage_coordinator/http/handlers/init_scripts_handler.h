#pragma once

#include <crow.h>

#include "../server.h"

namespace NDTS::NTabasco {

class TInitScriptsHandler {
public:
    void Handle(const crow::request& req, crow::response& res, const TContext& ctx);
};

} // end of NDTS::TTabasco namespace