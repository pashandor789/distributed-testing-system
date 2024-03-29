#include "server.h"

#include "handlers/build_handler.h"
#include "handlers/builds_handler.h"
#include "handlers/upload_task_root_dir.h"
#include "handlers/upload_tests_handler.h"
#include "handlers/upload_build_handler.h"

namespace NDTS::NTabasco {

using THandlerCallback = std::function<crow::response(const crow::request&)>;

template <typename THandler>
THandlerCallback GetHandlerCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
        [server](const crow::request& req) -> crow::response {
            THandler handler{};
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
    CROW_ROUTE(app_, "/uploadTests").methods("PUT"_method) (
        GetHandlerCallback<TUploadTestsHandler>(this)
    );

    CROW_ROUTE(app_, "/uploadTaskRootDir").methods("PUT"_method) (
        GetHandlerCallback<TUploadTaskRootDirHandler>(this)
    );

    CROW_ROUTE(app_, "/uploadBuild").methods("PUT"_method) (
        GetHandlerCallback<TUploadBuildHandler>(this)
    );

    CROW_ROUTE(app_, "/builds").methods("GET"_method) (
        GetHandlerCallback<TBuildsHandler>(this)
    );

    CROW_ROUTE(app_, "/build").methods("GET"_method) (
        GetHandlerCallback<TBuildHandler>(this)
    );
}

void TTabascoHTTPServer::Run() {
    app_.run();
}

} // end of NDTS::TTabasco namespace
