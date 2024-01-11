#pragma once

#include <optional>
#include <string>
#include <memory>

#include "common/proto/storage_client.pb.h"

namespace NDTS::NTabasco {

/* All methods of class return true on success, false otherwise */

struct File {
    std::string name;
    std::string content;
};

class TStorageClientImpl {
public:
    TStorageClientImpl(const TStorageClientConfig& config);

    bool UpsertData(const std::string& bucketName, const std::string& fileName, std::string data);

    bool UpdateData(const std::string &bucketName, const std::string &fileName, std::string data);

    bool InsertData(const std::string& bucketName, const std::string& fileName, std::string data);

    /* creates entity for storing data */
    bool CreateBucket(const std::string& bucketName);

    std::optional<std::string> GetData(const std::string& bucketName, const std::string& fileName);

    std::vector<File> GetFiles(const std::string& bucketName);

private:
    class TImpl;

private:
    std::shared_ptr<TImpl> pImpl_;
};

} // end of NDTS::NTabasco namespace
