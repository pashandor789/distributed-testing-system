#pragma once

#include <crow.h>

#include "../server.h"

namespace NDTS::NTabasco {

class TUploadTaskRootDirHandler {
public:
    void Handle(const crow::request& req, crow::response& res, const TContext& ctx);

    bool Parse(const crow::request& req, crow::response& res);

private:
    uint64_t taskId_;
    std::string rootDirZipData_;
};

} // end of NDTS::TTabasco namespace