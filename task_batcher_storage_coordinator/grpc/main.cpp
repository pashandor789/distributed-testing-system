#include <argparse/argparse.hpp>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <fstream>

#include "server.h"
#include "common/proto/tabasco_config.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

int main(int argc, char** argv) {
    argparse::ArgumentParser parser("http tabasco server");

    parser
        .add_argument("-c", "--config-path")
        .required()
        .help("specify config file");

    parser
        .add_argument("-p", "--server-port")
        .help("server port")
        .scan<'i', uint16_t>();

    parser.parse_args(argc, argv);

    auto configPath = parser.get<std::string>("--config-path");

    NDTS::NTabasco::TTabascoServerConfig config;

    std::fstream input(configPath, std::ios::in);
    google::protobuf::io::IstreamInputStream stream(&input);

    if (!input.is_open() ||!google::protobuf::TextFormat::Parse(&stream, &config)) {
        throw std::runtime_error("can't parse config");
    }

    if (std::optional<uint16_t> serverPort = parser.present<uint16_t>("--server-port")) {
        config.set_server_port(*serverPort);
    }

    std::string ipAddr;

    ipAddr
        .append("0.0.0.0")
        .append(":")
        .append(std::to_string(config.server_port()));

    NDTS::NTabasco::TTabascoGRPCServiceImpl service(config);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(ipAddr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << ipAddr << std::endl;
    server->Wait();
}
