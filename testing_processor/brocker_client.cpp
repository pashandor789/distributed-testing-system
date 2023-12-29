#include "brocker_client.h"

#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>

#include <nlohmann/json.hpp>

#include "testing_processor.h"
#include "testing_processor_request.h"

namespace NDTS::NTestingProcessor {

TBrockerClient::TBrockerClient(const TTestingProcessorConfig& config)
    : serverURL_(config.brocker_client_config().server_url())
    , queueName_(config.brocker_client_config().queue_name())
    , testingProcessor_(config)
{}

void TBrockerClient::Run() {
    auto *loop = ev_loop_new(0);

    AMQP::LibEvHandler handler(loop);
    AMQP::TcpConnection connection(&handler, AMQP::Address(serverURL_.c_str()));

    AMQP::TcpChannel channel(&connection);

    channel.declareQueue(queueName_);
    channel.setQos(1);

    channel.consume(queueName_).onReceived(
        [&channel, this](const AMQP::Message& message, uint64_t deliveryTag, bool redelivered) {
            std::string body(message.body(), message.bodySize());

            auto requestJson = nlohmann::json::parse(std::move(body), nullptr, false);
            auto request = TTestingProcessorRequest(requestJson);
            
            this->testingProcessor_.Process(std::move(request));

            channel.ack(deliveryTag);
        }
    );

    ev_run(loop, 0);
}

} // end of NDTS::NTestingProcessor namespace
