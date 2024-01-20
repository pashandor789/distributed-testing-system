#pragma once

#include <crow.h>

#include "../server.h"

namespace NDTS::NTabasco {

class TBuildHandler {
public:
    bool Parse(const crow::request& req, crow::response& res);

    void Handle(const crow::request& req, crow::response& res, const TContext& ctx);

private:
    uint64_t buildId_;
};

} // end of NDTS::TTabasco namespace