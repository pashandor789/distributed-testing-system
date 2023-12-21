#include "storage_client.h"

namespace NDTS::NTabasco {

TStorageClient::TStorageClient(const TStorageClientConfig& config)
    : baseURL_(config.url(), false)
    , provider_(config.access_key(), config.secret_key())
    , client_(baseURL_, &provider_) 
{
}

bool TStorageClient::UploadData(const std::string& bucketName, const std::string& fileName, std::string data) {
    size_t uploadSize = data.size();
    std::stringstream stream(std::move(data));

    minio::s3::PutObjectArgs args(stream, uploadSize, 0);
    args.bucket = bucketName;
    args.object = fileName;

    minio::s3::PutObjectResponse resp = client_.PutObject(args);

    return static_cast<bool>(resp);
}

bool TStorageClient::CreateBucket(const std::string& bucketName) {
    minio::s3::MakeBucketArgs args;
    args.bucket = bucketName;
    minio::s3::MakeBucketResponse resp = client_.MakeBucket(args);
    
    return static_cast<bool>(resp);
}

std::pair<bool, std::string> TStorageClient::GetData(const std::string& bucketName, const std::string& fileName) {
    minio::s3::GetObjectArgs args;
    args.bucket = bucketName;
    args.object = fileName;

    std::string data{};
    args.datafunc = 
    [&data](minio::http::DataFunctionArgs args) -> bool {
        data.append(std::move(args.datachunk));
        return true;
    };

    minio::s3::GetObjectResponse resp = client_.GetObject(args);

    return {static_cast<bool>(resp), data};
}

} // end of NDTS::NTabasco namespace
