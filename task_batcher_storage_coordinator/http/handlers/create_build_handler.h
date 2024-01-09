#pragma once

#include <crow.h>

#include "../server.h"

namespace NDTS::NTabasco {

class TCreateBuildHandler {
public:
    void Handle(const crow::request& req, crow::response& res, const TContext& ctx);

    bool Parse(const crow::request& req, crow::response& res);

private:
    std::string buildName_;
    std::string executeScript_;
    std::string initScript_;
    std::string description_;
};

} // end of NDTS::TTabasco namespace