#pragma once

#include <crow.h>

#include "../server.h"

namespace NDTS::NTabasco {

class TUploadBuildHandler {
public:
    void Handle(const crow::request& req, crow::response& res, const TContext& ctx);

    bool Parse(const crow::request& req, crow::response& res);

private:
    TBuild build_;
};

} // end of NDTS::TTabasco namespace