#include "server.h"

#include "handlers/builds_handler.h"
#include "handlers/create_build_handler.h"
#include "handlers/upload_execute_script_handler.h"
#include "handlers/upload_init_script_handler.h"
#include "handlers/upload_tests_handler.h"

namespace NDTS::NTabasco {

using THandlerCallback = std::function<crow::response(const crow::request&)>;

template <typename THandler>
THandlerCallback GetHandlerCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        THandler handler;
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
    CROW_ROUTE(app_, "/uploadTests").methods("POST"_method) (
        GetHandlerCallback<TUploadTestsHandler>(this)
    );

    CROW_ROUTE(app_, "/uploadInitScript").methods("POST"_method) (
        GetHandlerCallback<TUploadInitScriptHandler>(this)
    );

    CROW_ROUTE(app_, "/uploadExecuteScript").methods("POST"_method) (
        GetHandlerCallback<TUploadExecuteScriptHandler>(this)
    );

    CROW_ROUTE(app_, "/createBuild").methods("POST"_method) (
        GetHandlerCallback<TCreateBuildHandler>(this)
    );

    CROW_ROUTE(app_, "/builds").methods("GET"_method) (
        GetHandlerCallback<TBuildsHandler>(this)
    );
}

void TTabascoHTTPServer::Run() {
    app_.run();
}

} // end of NDTS::TTabasco namespace
