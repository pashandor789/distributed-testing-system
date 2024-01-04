#include "server.h"

#include "handler_callbacks.h"

namespace NDTS::NTabasco {

TTabascoHTTPServer::TTabascoHTTPServer(const TTabascoServerConfig& config)
    : batchSize_(config.batch_size())
    , storageClient_(config.storage_client_config())
    , builds_(config.build_data_base_config())
{
    InitHandlers();

    app_.concurrency(config.server_thread_count());
    app_.port(config.server_port());
}

void TTabascoHTTPServer::InitHandlers() {
    CROW_ROUTE(app_, "/uploadTests").methods("POST"_method) (
        GetLoadTestsCallback(this)
    );

    CROW_ROUTE(app_, "/uploadInitScript").methods("POST"_method) (
        GetUploadInitScriptCallback(this)
    );

    CROW_ROUTE(app_, "/uploadExecuteScript").methods("POST"_method) (
        GetUploadExecuteScriptCallback(this)
    );

    CROW_ROUTE(app_, "/createBuild").methods("POST"_method) (
        GetCreateBuildCallback(this)
    );

    CROW_ROUTE(app_, "/initScripts").methods("GET"_method) (
        GetInitScriptsCallback(this)
    );

    CROW_ROUTE(app_, "/executeScripts").methods("GET"_method) (
        GetExecuteScriptsCallback(this)
    );
}

void TTabascoHTTPServer::Run() {
    app_.run();
}

} // end of NDTS::TTabasco namespace
