#include <argparse/argparse.hpp>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "common/proto/tabasco_config.pb.h"

int main(int argc, char** argv) {
    argparse::ArgumentParser parser("http tabasco server");

    parser
        .add_argument("-c", "--config-path")
        .required()
        .help("specify config file");


    parser.parse_args(argc, argv);

    auto configPath = parser.get<std::string>("--config-path");

    NDTS::NTabasco::TTabascoServerConfig config;

    std::fstream input(configPath, std::ios::in);
    google::protobuf::io::IstreamInputStream stream(&input);

    if (!google::protobuf::TextFormat::Parse(&stream, &config)) {
        throw std::runtime_error("can't parse config");
    }
    
    // NDTS::NTabasco::TTabascoHTTPServer server{config};
    // server.Run();
}
