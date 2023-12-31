#include "../storage_client.h"

#include <catch2/catch_test_macros.hpp>

using namespace NDTS::NTabasco;

TStorageClientConfig GetConfig() {
    TStorageClientConfig config;

    config.set_uri("mongodb://localhost:27017");
    config.set_dbname("storage");

    return config;
}

TEST_CASE("[TStorageClient] Correctness.") {
    TStorageClient client(GetConfig());

    REQUIRE(client.UploadData("testBucket", "testFileName", "testContent"));

    auto [success, data] = client.GetData("testBucket", "testFileName");

    REQUIRE(success);
    REQUIRE(data == "testContent");
    REQUIRE(false);
}

TEST_CASE("[TStorageClient] Last write wins.") {
    TStorageClient client(GetConfig());

    REQUIRE(client.UploadData("test", "GladValakas", "1488"));
    REQUIRE(client.UploadData("test", "GladValakas", "1234"));
    REQUIRE(client.UploadData("test", "GladValakas", "2228"));

    auto [success, data] = client.GetData("test", "GladValakas");

    REQUIRE(success);
    REQUIRE(data == "2228");
}