#pragma once

#include <crow.h>
#include <miniocpp/client.h>

#include "common/proto/tabasco_config.pb.h"

namespace NDTS::NTabasco {

struct TContext {
    size_t batchSize;
    minio::s3::Client& storageClient; 
};

class TTabascoHTTPServer {
public:
    TTabascoHTTPServer(const TTabascoHTTPServerConfig& config);

    void Run();
    
private:
    void InitHandlers();

private:
    size_t batchSize_;

    minio::s3::BaseUrl baseURL_;
    minio::creds::StaticProvider provider_;
    minio::s3::Client client_; // dfs

    crow::SimpleApp app_; // http server
};

} // end of NDTS::TTabasco namespace
