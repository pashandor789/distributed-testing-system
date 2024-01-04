#include "handler_callbacks.h"

#include "handlers/execute_scripts_handler.h"
#include "handlers/init_scripts_handler.h"
#include "handlers/create_build_handler.h"
#include "handlers/upload_execute_script_handler.h"
#include "handlers/upload_init_script_handler.h"
#include "handlers/upload_tests_handler.h"

namespace NDTS::NTabasco {

THandlerCallback GetLoadTestsCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        TUploadTestsHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };

    return callback;
}

THandlerCallback GetUploadInitScriptCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        TUploadInitScriptHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };

    return callback;
}

THandlerCallback GetUploadExecuteScriptCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        TUploadExecuteScriptHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };

    return callback;
}

THandlerCallback GetCreateBuildCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        TCreateBuildHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };

    return callback;
}

THandlerCallback GetInitScriptsCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        TInitScriptsHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };

    return callback;
}

THandlerCallback GetExecuteScriptsCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        TExecuteScriptsHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };

    return callback;
}

} // end of NDTS::TTabasco namespace
