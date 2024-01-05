#include "../build_data_base.h"

#include <catch2/catch_test_macros.hpp>

#include <optional>

using namespace NDTS::NTabasco;

TBuildDataBaseConfig GetConfig() {
    TBuildDataBaseConfig config;

    config.set_dbname("postgres");
    config.set_host("localhost");
    config.set_port(5432);

    config.set_user("postgres");
    config.set_password("1488");

    return config;
}

TEST_CASE("[TBuildDataBase] Correctness.") {
    TBuildDataBase buildDataBase(GetConfig());

    TInitScript initScript{
        .id = 0,
        .name = "unitTestInit",
        .content = "echo 'hello, init!'"
    };

    {
        REQUIRE(buildDataBase.UploadInitScript(initScript.name, initScript.content));
        auto scripts = buildDataBase.GetInitScripts();

        auto foundIt = std::find_if(
            scripts.items.begin(),
            scripts.items.end(),
            [&](const TInitScript& rhsInitScript){
                return
                    initScript.name == rhsInitScript.name &&
                    initScript.content == rhsInitScript.content;
            }
        );

        REQUIRE(foundIt != scripts.items.end());

        initScript.id = foundIt->id;
    }

    TExecuteScript executeScript{
        .id = 0,
        .name = "unitTestExecute",
        .content = "echo 'hello, execute!'"
    };

    {
        REQUIRE(buildDataBase.UploadExecuteScript(executeScript.name, executeScript.content));
        auto scripts = buildDataBase.GetExecuteScripts();

        auto foundIt = std::find_if(
                scripts.items.begin(),
                scripts.items.end(),
                [&](const TExecuteScript& rhsExecuteScript){
                    return
                        executeScript.name == rhsExecuteScript.name &&
                        executeScript.content == rhsExecuteScript.content;
                }
        );

        REQUIRE(foundIt != scripts.items.end());

        executeScript.id = foundIt->id;
    }

    TBuild build {
        .id = 0,
        .name = "unitTestBuild",
        .initScript = initScript,
        .executeScript = executeScript
    };

    {
        REQUIRE(buildDataBase.CreateBuild(build.name, build.executeScript.id, build.initScript.id));

        auto builds = buildDataBase.GetBuilds();

        auto foundIt = std::find_if(
                builds.items.begin(),
                builds.items.end(),
                [&](const TBuild& rhsBuild){
                    return
                        build.initScript.id == rhsBuild.initScript.id &&
                        build.executeScript.id == rhsBuild.executeScript.id &&
                        build.name == rhsBuild.name;
                }
        );

        REQUIRE(foundIt != builds.items.end());

        build.id = foundIt->id;
    }
}
