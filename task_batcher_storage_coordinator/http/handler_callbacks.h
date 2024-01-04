#pragma once

#include <functional>

#include "server.h"

namespace NDTS::NTabasco {

using THandlerCallback = std::function<crow::response(const crow::request&)>;

THandlerCallback GetLoadTestsCallback(TTabascoHTTPServer* server);

THandlerCallback GetUploadInitScriptCallback(TTabascoHTTPServer* server);

THandlerCallback GetUploadExecuteScriptCallback(TTabascoHTTPServer* server);

THandlerCallback GetCreateBuildCallback(TTabascoHTTPServer* server);

THandlerCallback GetInitScriptsCallback(TTabascoHTTPServer* server);

THandlerCallback GetExecuteScriptsCallback(TTabascoHTTPServer* server);

} // end of NDTS::TTabasco namespace
