#include "server.h"

#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

#include "common/proto/batch.pb.h"

#include "load_tests_handler.h"

namespace NDTS::TTabasco {

TTabascoHTTPServer::TTabascoHTTPServer(const TTabascoHTTPServerConfig& config)
    : batchSize_(config.batchSize())
    , baseURL_(config.storageURL(), false)
    , provider_(config.storageAccessKey(), config.storageSecretKey())
    , client_(baseURL_, &provider_)
{
    InitHandlers();

    app_.concurrency(config.serverThreadCount());
    app_.port(config.serverPort());
}

void TTabascoHTTPServer::InitHandlers() {
    CROW_ROUTE(app_, "/loadTests")(
        [this](const crow::request& req, crow::response& res) {
            TLoadTestsHandler handler;
            handler.Handle(req, res, {.batchSize = batchSize_, .storageClient = client_});
        };
    );
}

void TTabascoHTTPServer::Run() {
    app_.run();
}

} // end of NDTS::TTabasco namespace
