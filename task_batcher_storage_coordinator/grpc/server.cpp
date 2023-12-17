#include "server.h"

namespace NDTS::NTabasco {

Status TTabascoGRPCServiceImpl::GetBatch(ServerContext* context, const TGetBatchRequest* request, TGetBatchResponse* reply) {
    
    
    minio::s3::GetObjectArgs args;
    args.bucket = std::to_string(request.)
}

Status TTabascoGRPCServiceImpl::GetScripts(ServerContext* context, const TGetScriptsRequest* request, TGetScriptsResponse* reply) {
    
}

} // end of NDTS::NTabasco namespace