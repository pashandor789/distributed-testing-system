#pragma once 

#include <pqxx/pqxx>
#include "common/proto/build_data_base.pb.h"

namespace NDTS::NTabasco {

class TBuildDataBase {
public:
    TBuildDataBase(const TBuildDataBaseConfig& config);

    void UploadInitScript(std::string scriptName, std::string content);

    void UploadExecuteScript(std::string scriptName, std::string content);

    void CreateBuild(uint64_t executeScriptId, uint64_t initScriptId);

private:
    pqxx::connection connection_;
};

} // end of NDTS::TTabasco namespace
