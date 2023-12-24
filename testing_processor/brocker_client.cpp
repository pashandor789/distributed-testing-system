#include "brocker_client.h"

#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

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
    auto evbase = event_base_new();

    AMQP::LibEventHandler handler(evbase);
    AMQP::TcpConnection connection(&handler, AMQP::Address(serverURL_.c_str()));

    AMQP::TcpChannel channel(&connection);

    channel.declareQueue(queueName_);
    channel.setQos(1);

    channel.consume(queueName_).onReceived(
        [&channel, this](const AMQP::Message& message, uint64_t deliveryTag, bool redelivered) {
            std::cerr << "recievied" << std::endl;

            std::string body(message.body(), message.bodySize());

            auto requestJson = nlohmann::json::parse(std::move(body), nullptr, false);
            auto request = TTestingProcessorRequest(requestJson);
            
            this->testingProcessor_.Process(std::move(request));

            channel.ack(deliveryTag);
        }
    );


    event_base_dispatch(evbase);
    event_base_free(evbase);
}

} // end of NDTS::NTestingProcessor namespace
