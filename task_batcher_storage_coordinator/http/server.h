#pragma once

#include <crow.h>

#include "common/proto/tabasco_config.pb.h"
#include "common/storage_client/storage_client.h"

namespace NDTS::NTabasco {

class TTabascoHTTPServer {
public:
    TTabascoHTTPServer(const TTabascoServerConfig& config);

    void Run();
    
private:
    void InitHandlers();
    
public:
    size_t batchSize_;
    TStorageClient storageClient_;
    crow::SimpleApp app_; // http server
};

struct TContext {
    TTabascoHTTPServer* server;
};

} // end of NDTS::TTabasco namespace
