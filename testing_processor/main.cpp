#include "brocker_client.h"

#include <argparse/argparse.hpp>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <thread>

int main(int argc, char** argv) {
    argparse::ArgumentParser parser("testing processor");

    parser
        .add_argument("-c", "--config-path")
        .required()
        .help("specify config file");


    parser.parse_args(argc, argv);

    auto configPath = parser.get<std::string>("--config-path");

    NDTS::NTestingProcessor::TTestingProcessorConfig config;

    std::fstream input(configPath, std::ios::in);
    google::protobuf::io::IstreamInputStream stream(&input);

    if (!google::protobuf::TextFormat::Parse(&stream, &config)) {
        throw std::runtime_error("can't parse config");
    }

    std::vector<std::thread> testingProcessors;
    testingProcessors.reserve(config.instance_count());

    for (size_t i = 0; i < config.instance_count(); ++i) {
        testingProcessors.push_back(
            [&config]() {
                TBrockerClient client(config);
                client.Run();
            }
        );
    }

    testingProcessors.back().join();
}