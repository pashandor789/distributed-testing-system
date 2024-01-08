#pragma once 

#include <pqxx/pqxx>
#include <optional>

#include "common/proto/build_data_base.pb.h"

#include "data_repr.h"

namespace NDTS::NTabasco {

class TBuildDataBase {
public:
    TBuildDataBase(const TBuildDataBaseConfig& config);

    bool UploadInitScript(std::string scriptName, std::string content);

    bool UploadExecuteScript(std::string scriptName, std::string content);

    bool CreateBuild(std::string buildName, uint64_t executeScriptId, uint64_t initScriptId);

    std::optional<TScripts> GetScripts(uint64_t buildId);

    TExecuteScripts GetExecuteScripts();

    TInitScripts GetInitScripts();

    TBuilds GetBuilds();

private:
    pqxx::connection connection_;
};

} // end of NDTS::TTabasco namespace
