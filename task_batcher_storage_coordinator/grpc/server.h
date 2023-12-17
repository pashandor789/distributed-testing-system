#include "proto/tabasco_grpc.grpc.pb.h"

#include <miniocpp/client.h>

namespace NDTS::NTabasco {

class TTabascoGRPCServiceImpl final: public TTabascoGRPC::Service {
public:
    Status GetBatch(ServerContext* context, const TGetBatchRequest* request, TGetBatchResponse* reply) override;

    Status GetScripts(ServerContext* context, const TGetScriptsRequest* request, TGetScriptsResponse* reply) override;

public:
    minio::s3::BaseUrl baseURL_;
    minio::creds::StaticProvider provider_;
    minio::s3::Client client_; // dfs

};

} // end of NDTS::NTabasco namespace