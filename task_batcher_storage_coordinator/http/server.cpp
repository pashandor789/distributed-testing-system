#include "server.h"

#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

#include "load_tests_handler.h"

namespace NDTS::NTabasco {

TTabascoHTTPServer::TTabascoHTTPServer(const TTabascoHTTPServerConfig& config)
    : batchSize_(config.batch_size())
    , baseURL_(config.storage_url(), false)
    , provider_(config.storage_access_key(), config.storage_secret_key())
    , client_(baseURL_, &provider_)
{
    InitHandlers();

    app_.concurrency(config.server_thread_count());
    app_.port(config.server_port());
}

void TTabascoHTTPServer::InitHandlers() {
    CROW_ROUTE(app_, "/loadTests")(
        [this](const crow::request& req, crow::response& res) {
            TLoadTestsHandler handler;
            handler.Handle(req, res, {.batchSize = batchSize_, .storageClient = client_});
        }
    );
}

void TTabascoHTTPServer::Run() {
    app_.run();
}

} // end of NDTS::TTabasco namespace
