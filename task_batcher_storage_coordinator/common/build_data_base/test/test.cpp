#include "../build_data_base.h"

#include <catch2/catch_test_macros.hpp>

using namespace NDTS::NTabasco;

TBuildDataBaseConfig GetConfig() {
    TBuildDataBaseConfig config;

    config.set_dbname("postgres");
    config.set_hostaddr("127.0.0.1");
    config.set_port(5432);

    config.set_user("postgres");
    config.set_password("1488");

    return config;
}

TEST_CASE("[TBuildDataBase] Correctness.") {
    TBuildDataBase buildDataBase(GetConfig());

    uint64_t initScriptId = buildDataBase.UploadInitScript("testInitName", " echo 'hello, init!' ");
    uint64_t execScriptId = buildDataBase.UploadExecuteScript("testScriptName", " echo 'hello, execute!' ");

    uint64_t buildId = buildDataBase.CreateBuild("testBuild", execScriptId, initScriptId);

    auto scripts = buildDataBase.GetScripts(buildId);

    REQUIRE(scripts.executeScript == " echo 'hello, execute!' ");
    REQUIRE(scripts.initScript == " echo 'hello, init!' ");
}
