#include "build_data_base.h"

namespace NDTS::NTabasco {

std::string GetAuthenticationString(const TBuildDataBaseConfig& config) {
    std::string authenticationString;

    authenticationString
        .append("dbname = ").append(config.dbname()).append(" ")
        .append("user = ").append(config.user()).append(" ")
        .append("password = ").append(config.password()).append(" ")
        .append("hostaddr = ").append(config.hostaddr()).append(" ")
        .append("port = ").append(config.port()).append(" ");

    return authenticationString;
}

TBuildDataBase::TBuildDataBase(const TBuildDataBaseConfig& config)
    : connection_(GetAuthenticationString(config))
{
    if (!connection_.is_open()) {
        throw std::runtime_error("db connection error");
    }
}

void TBuildDataBase::UploadInitScript(std::string scriptName, std::string content) {
    pqxx::nontransaction nonTx(connection_);
    nonTx.exec_params("INSERT INTO initScripts (scriptName, content) VALUES ($1, $2)", scriptName, content);
}

void TBuildDataBase::UploadExecuteScript(std::string scriptName, std::string content) {
    pqxx::nontransaction nonTx(connection_);
    nonTx.exec_params("INSERT INTO executeScripts (scriptName, content) VALUES ($1, $2)", scriptName, content);
}

void TBuildDataBase::CreateBuild(uint64_t executeScriptId, uint64_t initScriptId) {
    pqxx::nontransaction nonTx(connection_);
    nonTx.exec_params("INSERT INTO builds (scriptName, content) VALUES ($1, $2)", executeScriptId, initScriptId);
}

} // end of NDTS::TTabasco namespace
