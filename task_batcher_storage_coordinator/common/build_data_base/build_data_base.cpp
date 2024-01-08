#include "build_data_base.h"

namespace NDTS::NTabasco {

std::string GetAuthenticationString(const TBuildDataBaseConfig& config) {
    std::string authenticationString;

    authenticationString
        .append("dbname = ").append(config.dbname()).append(" ")
        .append("user = ").append(config.user()).append(" ")
        .append("password = ").append(config.password()).append(" ")
        .append("host = ").append(config.host()).append(" ")
        .append("port = ").append(std::to_string(config.port())).append(" ");

    return authenticationString;
}

TBuildDataBase::TBuildDataBase(const TBuildDataBaseConfig& config)
    : connection_(GetAuthenticationString(config))
{
    if (!connection_.is_open()) {
        throw std::runtime_error("db connection error");
    }
}

bool TBuildDataBase::UploadInitScript(std::string scriptName, std::string content) {
    try {
        pqxx::nontransaction nonTx(connection_);
        nonTx.exec_params("INSERT INTO init_scripts (name, content) VALUES ($1, $2)", std::move(scriptName), std::move(content));
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool TBuildDataBase::UploadExecuteScript(std::string scriptName, std::string content) {
    try {
        pqxx::nontransaction nonTx(connection_);
        nonTx.exec_params("INSERT INTO execute_scripts (name, content) VALUES ($1, $2)", std::move(scriptName), std::move(content));
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool TBuildDataBase::CreateBuild(std::string buildName, uint64_t executeScriptId, uint64_t initScriptId) {
    try {
        pqxx::nontransaction nonTx(connection_);
        nonTx.exec_params(
        " INSERT INTO builds "
            " (name, init_script_id, execute_script_id) "
            " VALUES ($1, $2, $3) ", std::move(buildName), initScriptId, executeScriptId
        );
        return true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

std::optional<TScripts> TBuildDataBase::GetScripts(uint64_t buildId) {
    try {
        pqxx::nontransaction nonTx(connection_);
        pqxx::result scriptsId = nonTx.exec_params("SELECT * FROM builds WHERE id = $1", buildId);

        uint64_t initScriptId = scriptsId[0][2].as<uint64_t>();
        pqxx::result initScriptRows = nonTx.exec_params("SELECT * FROM init_scripts WHERE id = $1", initScriptId);

        uint64_t executeScriptId = scriptsId[0][3].as<uint64_t>();
        pqxx::result executeScriptRows = nonTx.exec_params("SELECT * FROM execute_scripts WHERE id = $1", executeScriptId);

        return TScripts{
            .initScript = initScriptRows[0][2].as<std::string>(),
            .executeScript = executeScriptRows[0][2].as<std::string>()
        };
    } catch (std::exception& e) {
        return std::nullopt;
    }
}

TInitScripts TBuildDataBase::GetInitScripts() {
    pqxx::nontransaction nonTx(connection_);

    std::vector<TInitScript> initScripts;

    pqxx::result deserializedScripts = nonTx.exec("SELECT id, name, content FROM init_scripts");
    initScripts.reserve(deserializedScripts.size());

    for (const auto& script: deserializedScripts) {
        initScripts.push_back(
            TInitScript{
                .id = script[0].as<uint64_t>(),
                .name = script[1].as<std::string>(),
                .content = script[2].as<std::string>()
            }
        );
    }

    return TInitScripts{.items = std::move(initScripts)};
}

TExecuteScripts TBuildDataBase::GetExecuteScripts() {
    pqxx::nontransaction nonTx(connection_);

    std::vector<TExecuteScript> executeScripts;

    pqxx::result deserializedScripts = nonTx.exec("SELECT id, name, content FROM execute_scripts");
    executeScripts.reserve(deserializedScripts.size());

    for (const auto& script: deserializedScripts) {
        executeScripts.push_back(
            TExecuteScript{
                .id = script[0].as<uint64_t>(),
                .name = script[1].as<std::string>(),
                .content = script[2].as<std::string>()
            }
        );
    }

    return TExecuteScripts{.items = std::move(executeScripts)};
}

TBuilds TBuildDataBase::GetBuilds() {
    pqxx::nontransaction nonTx(connection_);

    std::vector<TBuild> builds;

    pqxx::result deserializedBuilds = nonTx.exec(
        "SELECT builds.id, builds.name, "
        " init_scripts.id, init_scripts.name, init_scripts.content, "
        " execute_scripts.id, execute_scripts.name, execute_scripts.content "
        " FROM builds "
        " JOIN init_scripts ON builds.init_script_id = init_scripts.id "
        " JOIN execute_scripts ON builds.execute_script_id = execute_scripts.id "
    );

    for (const auto& script: deserializedBuilds) {
        auto initScript =
            TInitScript {
                    .id = script[2].as<uint64_t>(),
                    .name = script[3].as<std::string>(),
                    .content = script[4].as<std::string>()
            };

        auto executeScript =
            TExecuteScript {
                .id = script[5].as<uint64_t>(),
                .name = script[6].as<std::string>(),
                .content = script[7].as<std::string>()
            };

        builds.push_back(
            TBuild{
                .id = script[0].as<uint64_t>(),
                .name = script[1].as<std::string>(),
                .initScript = std::move(initScript),
                .executeScript = std::move(executeScript)
            }
        );
    }

    return {.items = std::move(builds)};
}

} // end of NDTS::TTabasco namespace
