#include "handler_callbacks.h"

#include "load_tests_handler.h"

#include <nlohmann/json.hpp>

namespace NDTS::NTabasco {

THandlerCallback GetLoadTestsCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback = 
    [server](const crow::request& req) -> crow::response {
        TLoadTestsHandler handler;
        crow::response resp;
        handler.Handle(req, resp, TContext{.server = server});
        return resp;
    };
    
    return callback;
}

THandlerCallback GetUploadInitScriptCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        crow::response resp;

        nlohmann::json data = nlohmann::json::parse(req.body, nullptr, false);

        if (data.is_discarded() || !data.contains("scriptName") || !data.contains("content")) {
            resp.code = 400;
            resp.body = "bad json";
            return resp;
        }

        server->builds_.UploadInitScript(std::move(data["scriptName"]), std::move(data["content"]));

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

        server->builds_.UploadExecuteScript(std::move(data["scriptName"]), std::move(data["content"]));

        return resp;
    };

    return callback;
}

THandlerCallback GetCreateBuildCallback(TTabascoHTTPServer* server) {
    THandlerCallback callback =
    [server](const crow::request& req) -> crow::response {
        crow::response resp;

        nlohmann::json data = nlohmann::json::parse(req.body, nullptr, false);

        if (data.is_discarded() || !data.contains("executeScriptId") || !data.contains("initScriptId")) {
            resp.code = 400;
            resp.body = "bad json";
            return resp;
        }

        server->builds_.CreateBuild(data["executeScriptId"], data["initScriptId"]);

        return resp;
    };

    return callback;
}


} // end of NDTS::TTabasco namespace