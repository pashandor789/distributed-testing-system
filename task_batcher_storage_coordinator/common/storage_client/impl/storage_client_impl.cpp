#include "storage_client_impl.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace NDTS::NTabasco {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

static mongocxx::instance instance{};

class TStorageClientImpl::TImpl {
public:
    TImpl(const TStorageClientConfig& config)
        : client_(mongocxx::uri(config.uri()))
        , db_(client_.database(config.dbname()))
    {}

    bool UpsertData(const std::string& bucketName, const std::string& fileName, std::string data) {
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

    bool InsertData(const std::string& bucketName, const std::string& fileName, std::string data) {
        auto collection = db_.collection(bucketName);
        auto insertDocument = make_document(
            kvp("fileName", fileName),
            kvp("data", std::move(data))
        );

        collection.insert_one(insertDocument.view());

        return true;
    }

    bool CreateBucket(const std::string& bucketName) {
        return true;
    }

    std::optional<std::string> GetData(const std::string& bucketName, const std::string& fileName) {
        auto collection = db_.collection(bucketName);

        auto maybeResult = collection.find_one(make_document(kvp("fileName", fileName)));
        if (maybeResult) {
            bsoncxx::document::view view = maybeResult->view();
            std::string data = std::string(view["data"].get_string());
            return data;
        }

        return std::nullopt;
    }

    std::vector<File> GetFiles(const std::string& bucketName) {
        auto collection = db_.collection(bucketName);

        std::vector<File> files;

        for (auto& document: collection.find({})) {
            files.push_back({
                .name = std::string(document["fileName"].get_string()),
                .content = std::string(document["data"].get_string())
            });
        }

        return files;
    }

private:
    mongocxx::client client_;
    mongocxx::database db_;
};

TStorageClientImpl::TStorageClientImpl(const TStorageClientConfig& config)
    : pImpl_(std::make_shared<TImpl>(config))
{}

bool TStorageClientImpl::UpsertData(const std::string& bucketName, const std::string& fileName, std::string data) {
    return pImpl_->UpsertData(bucketName, fileName, std::move(data));
}

bool TStorageClientImpl::InsertData(const std::string& bucketName, const std::string& fileName, std::string data) {
    return pImpl_->InsertData(bucketName, fileName, std::move(data));
}

bool TStorageClientImpl::CreateBucket(const std::string& bucketName) {
    return pImpl_->CreateBucket(bucketName);
}

std::optional<std::string> TStorageClientImpl::GetData(const std::string& bucketName, const std::string& fileName) {
    return pImpl_->GetData(bucketName, fileName);
}

std::vector<File> TStorageClientImpl::GetFiles(const std::string& bucketName) {
    return pImpl_->GetFiles(bucketName);
}

} // end of NDTS::NTabasco namespace
