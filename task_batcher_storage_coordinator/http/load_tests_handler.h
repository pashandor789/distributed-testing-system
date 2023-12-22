#pragma once

#include <crow.h>

#include "server.h"

namespace NDTS::NTabasco {

class TLoadTestsHandler {
public:
    void Handle(const crow::request& req, crow::response& res, const TContext& ctx);

    bool Parse(const crow::request& req, crow::response& res);

private:
    std::string taskId_;
    std::vector<std::string> inputTests_;
    std::vector<std::string> outputTests_;
};

} // end of NDTS::TTabasco namespace
