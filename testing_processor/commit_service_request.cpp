#include "commit_service_request.h"

#include <utility>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

namespace NDTS::NTestingProcessor {

TCommitServiceRequest::TCommitServiceRequest(std::string url)
    : url_(std::move(url))
{}

TCommitServiceRequest::TCommitError TCommitServiceRequest::Commit(uint64_t submissionId, const std::vector<TTestingReport> &report) {
    nlohmann::json data;

    data["submissionId"] = submissionId;
    data["report"] = ToJSON(report);

    auto deserializedJson = data.dump();

    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(url_));

        std::list<std::string> header;
        header.emplace_back("Content-Type: application/json");

        request.setOpt(new curlpp::options::HttpHeader(header));

        request.setOpt(new curlpp::options::PostFields(deserializedJson));
        request.setOpt(new curlpp::options::PostFieldSize(deserializedJson.size()));

        request.perform();
    } catch (std::exception& exception) {
        return exception.what();
    }

    return std::nullopt;
}

} // end of NDTS::NTestingProcessor namespace