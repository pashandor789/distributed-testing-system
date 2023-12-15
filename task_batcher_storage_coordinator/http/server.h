#pragma once

#include <crow.h>
#include <miniocpp/client.h>

#include "common/proto/tabasco_config.pb.h"

namespace NDTS::TTabasco {

class TTabascoHTTPServer {
public:
    TTabascoHTTPServer(const TTabascoHTTPServerConfig& config);

    void Run();
    
private:
    void InitHandlers();

    auto GetLoadTestsHandler();

private:
    void UploadStorageTests(
        std::vector<std::string>&& tests,
        const std::string& testSuffix,
        const std::string& taskId
    );

    void UploadStorageBatches(
        std::vector<std::vector<uint64_t>&& batches
        const std::string& taskId
    );

    std::vector<std::vector<size_t>> SplitIntoBatches(
        const std::vector<std::string>& inputTests,
        const std::vector<std::string>& outputTests
    );

private:
    size_t batchSize_;

    minio::s3::BaseUrl baseURL_;
    minio::creds::StaticProvider provider_;
    minio::s3::Client client_; // dfs

    crow::SimpleApp app_; // http server
};

} // end of NDTS::TTabasco namespace
