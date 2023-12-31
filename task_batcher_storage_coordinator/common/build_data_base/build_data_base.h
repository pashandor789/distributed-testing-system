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

    /* return's id of inserted */
    uint64_t UploadInitScript(std::string scriptName, std::string content);

    /* return's id of inserted */
    uint64_t UploadExecuteScript(std::string scriptName, std::string content);

    /* return's id of inserted */
    uint64_t CreateBuild(std::string buildName, uint64_t executeScriptId, uint64_t initScriptId);

    TScripts GetScripts(uint64_t buildId);

private:
    pqxx::connection connection_;
};

} // end of NDTS::TTabasco namespace
