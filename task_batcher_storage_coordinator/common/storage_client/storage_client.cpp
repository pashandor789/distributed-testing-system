#include "storage_client.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

namespace NDTS::NTabasco {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

static mongocxx::instance instance{};

class TStorageClient::TImpl {
public:
    TImpl(const TStorageClientConfig& config)
        : client_(mongocxx::uri(config.uri()))
        , db_(client_.database(config.dbname()))
    {}

    bool UploadData(const std::string& bucketName, const std::string& fileName, std::string data) {
        auto collection = db_.collection(bucketName);

        auto upsertDocument = make_document(
            kvp("$set", make_document(kvp("data", std::move(data))))
        );

        auto filter = make_document(
            kvp("fileName", fileName)
        );

        auto options = mongocxx::options::update().upsert(true);

        collection.update_one(filter.view(), upsertDocument.view(), options);

        return true;
    }

    bool CreateBucket(const std::string& bucketName) {
        db_.create_collection(bucketName);
        return true;
    }

    std::pair<bool, std::string> GetData(const std::string& bucketName, const std::string& fileName) {
        auto collection = db_.collection(bucketName);

        auto maybeResult = collection.find_one(make_document(kvp("fileName", fileName)));
        if (maybeResult) {
            bsoncxx::document::view view = maybeResult->view();
            std::string data = std::string(view["data"].get_string());
            return std::make_pair(true, std::move(data));
        }

        return {false, ""};
    }

private:
    mongocxx::client client_;
    mongocxx::database db_;
};

TStorageClient::TStorageClient(const TStorageClientConfig& config)
    : pImpl_(std::make_shared<TImpl>(config))
{}

bool TStorageClient::UploadData(const std::string& bucketName, const std::string& fileName, std::string data) {
    return pImpl_->UploadData(bucketName, fileName, std::move(data));   
}

bool TStorageClient::CreateBucket(const std::string& bucketName) {
    return pImpl_->CreateBucket(bucketName);
}

std::pair<bool, std::string> TStorageClient::GetData(const std::string& bucketName, const std::string& fileName) {
    return pImpl_->GetData(bucketName, fileName);
}

} // end of NDTS::NTabasco namespace
