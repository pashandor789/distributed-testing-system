#pragma once

#include <crow.h>

#include <string>

#include "../server.h"

namespace NDTS::NTabasco {

class TUploadInitScriptHandler {
public:
    void Handle(const crow::request& req, crow::response& res, const TContext& ctx);

    bool Parse(const crow::request& req, crow::response& res);

private:
    std::string scriptName_;
    std::string content_;
};

} // end of NDTS::TTabasco namespace
