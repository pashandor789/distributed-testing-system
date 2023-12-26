#pragma once

#include <string>

#include <miniocpp/client.h>

#include "common/proto/storage_client.pb.h"

namespace NDTS::NTabasco {

class TStorageClient {
public:
    TStorageClient(const TStorageClientConfig& config);

    bool UploadData(const std::string& bucketName, const std::string& fileName, std::string data);

    bool CreateBucket(const std::string& bucketName);

    std::pair<bool, std::string> GetData(const std::string& bucketName, const std::string& fileName);

private:
    // TODO: do not use minio :)
    minio::s3::BaseUrl baseURL_;
    minio::creds::StaticProvider provider_;
    minio::s3::Client client_; // dfs
};

} // end of NDTS::NTabasco namespace
