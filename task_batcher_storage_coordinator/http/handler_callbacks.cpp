#include "handler_callbacks.h"

#include "handlers/upload_tests_handler.h"
#include "handlers/upload_init_script_handler.h"

#include <nlohmann/json.hpp>

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
        crow::response resp;

        nlohmann::json data = nlohmann::json::parse(req.body, nullptr, false);

        if (data.is_discarded() || !data.contains("scriptName") || !data.contains("content")) {
            resp.code = 400;
            resp.body = "bad json";
            return resp;
        }

        bool success =
            server->builds_.UploadExecuteScript(
                    std::move(data["scriptName"]),
                    std::move(data["content"])
            );

        if (!success) {
            resp.code = 500;
            resp.body = "build db error!";
            return resp;
        }

        return resp;
    };

    return callback;
}

THandlerCallback GetCreateBuildCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        crow::response resp;

        nlohmann::json data = nlohmann::json::parse(req.body, nullptr, false);

        bool isDataInvalid = 
            !data.contains("executeScriptId") || !data.contains("initScriptId") || !data.contains("buildName");

        if (data.is_discarded() || isDataInvalid) {
            resp.code = 400;
            resp.body = "bad json";
            return resp;
        }

        bool success =
            server->builds_.CreateBuild(
                    data["buildName"],
                    data["executeScriptId"],
                    data["initScriptId"]
            );

        if (!success) {
            resp.code = 500;
            resp.body = "build db error!";
            return resp;
        }

        return resp;
    };

    return callback;
}


} // end of NDTS::TTabasco namespace
