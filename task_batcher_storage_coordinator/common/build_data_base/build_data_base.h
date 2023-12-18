#pragma once 

#include <pqxx/pqxx>
#include "common/proto/build_data_base.pb.h"

namespace NDTS::NTabasco {

struct TScripts {
    std::string initScript;
    std::string executeScript;
};

class TBuildDataBase {
public:
    TBuildDataBase(const TBuildDataBaseConfig& config);

    void UploadInitScript(std::string scriptName, std::string content);

    void UploadExecuteScript(std::string scriptName, std::string content);

    void CreateBuild(uint64_t executeScriptId, uint64_t initScriptId);

    TScripts GetScripts(uint64_t buildId);

private:
    pqxx::connection connection_;
};

} // end of NDTS::TTabasco namespace
