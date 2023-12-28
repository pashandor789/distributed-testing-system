#pragma once

#include <string>

#include <memory>

#include "common/proto/storage_client.pb.h"

namespace NDTS::NTabasco {

/* All methods of class return true on success, false otherwise */

class TStorageClient {
public:
    TStorageClient(const TStorageClientConfig& config);

    /* works as upsert */
    bool UploadData(const std::string& bucketName, const std::string& fileName, std::string data);

    /* create's entity for storing tests */
    bool CreateBucket(const std::string& bucketName);

    std::pair<bool, std::string> GetData(const std::string& bucketName, const std::string& fileName);

private:
    class TImpl;

private:
    std::shared_ptr<TImpl> pImpl_;
};

} // end of NDTS::NTabasco namespace
