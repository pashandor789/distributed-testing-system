#pragma once

#include <memory>

namespace NDTS::NTestingProcessor {

class TTabascoRequestTask {
public:
    TTabascoRequestTask(std::shared_ptr<TTabascoGRPC::Stub> stub)
        : stub_(std::move(stub))
    {}

    void NTabasco::GetScriptsResponse GetScripts() {
        NTabasco::GetScriptsResponse response;
        grpc::ClientContext context;

        tabasco_->GetScripts(getScriptsRequest);
    }

private:
    std::shared_ptr<TTabascoGRPC::Stub> stub_;
};

} // end of NDTS::NTestingProcessor namespace