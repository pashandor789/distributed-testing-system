#include "proto/tabasco_grpc.pb.h"
#include "proto/tabasco_grpc.grpc.pb.h"

#include "common/proto/tabasco_config.pb.h"
#include "common/storage_client/storage_client.h"

namespace NDTS::NTabasco {

class TTabascoGRPCServiceImpl final: public TTabascoGRPC::Service {
public:
    TTabascoGRPCServiceImpl(const TTabascoServerConfig& config);

    grpc::Status GetBatch(grpc::ServerContext* context, const TGetBatchRequest* request, TGetBatchResponse* reply) override;

    grpc::Status GetScripts(grpc::ServerContext* context, const TGetScriptsRequest* request, TGetScriptsResponse* reply) override;

public:
    TStorageClient storageClient_;
};

} // end of NDTS::NTabasco namespace
