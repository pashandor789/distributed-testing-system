#include "brocker_client.h"

#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

#include <crow.h>

#include "testing_processor.h"
#include "testing_processor_request.h"

namespace NDTS::NTestingProcessor {

TBrockerClient::TBrockerClient(const TBrockerClientConfig& brockerConfig, const TTestingProcessorConfig testingProcessorConfig)
    : serverURL_(brockerConfig.serverurl())
    , queueName_(brockerConfig.queuename())
    , testingProcessor_(testingProcessorConfig)
{}

TBrockerClient::Run() {
    auto evbase = event_base_new();

    AMQP::LibEventHandler handler(evbase);
    AMQP::TcpConnection connection(&handler, AMQP::Address(serverURL_.c_str()));

    AMQP::TcpChannel channel(&connection);

    channel.declareQueue(queueName_);
    channel.setQos(1);

    channel.consume(queueName_)
        .onReceived([&](const AMQP::Message& message, uint64_t deliveryTag, bool redelivered) {
            auto requestJson = crow::json::load(message.body());
            auto request = TTestingProcessorRequest(requestJson);
            
            testingProcessor_.Process(request);

            channel.ack(deliveryTag);
        });


    event_base_dispatch(evbase);
    event_base_free(evbase);
}

} // end of NDTS::NTestingProcessor namespace
