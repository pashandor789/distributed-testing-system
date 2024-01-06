#include "broker_client.h"

#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>

#include <nlohmann/json.hpp>

#include <syncstream>

#include "testing_processor.h"
#include "testing_processor_request.h"

namespace NDTS::NTestingProcessor {

TBrokerClient::TBrokerClient(const TTestingProcessorConfig& config)
    : serverURL_(config.broker_client_config().server_url())
    , queueName_(config.broker_client_config().queue_name())
    , testingProcessor_(config)
{}

void TBrokerClient::Run() {
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


    std::cerr << "Started instance of TBrokerClient with TTestingProcessor" << std::endl;
    ev_run(loop, 0);

    throw std::runtime_error("Connection error detected!");
}

} // end of NDTS::NTestingProcessor namespace
