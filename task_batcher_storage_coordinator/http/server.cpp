#include "server.h"

#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

#include "load_tests_handler.h"

namespace NDTS::NTabasco {

auto GetLoadTestsHandler(TTabascoHTTPServer* server) {
    auto callback = 
    [server](const crow::request& req) {
        TLoadTestsHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };
    
    return callback;
}

TTabascoHTTPServer::TTabascoHTTPServer(const TTabascoServerConfig& config)
    : batchSize_(config.batch_size())
    , storageClient_(config.storage_client_config())
{
    InitHandlers();

    app_.concurrency(config.server_thread_count());
    app_.port(config.server_port());
}

void TTabascoHTTPServer::InitHandlers() {
    CROW_ROUTE(app_, "/loadTests").methods("POST"_method)(
        GetLoadTestsHandler(this)
    );

}

void TTabascoHTTPServer::Run() {
    app_.run();
}

} // end of NDTS::TTabasco namespace
