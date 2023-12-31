#include <argparse/argparse.hpp>

#include "executor.h"

int main(int argc, char** argv) {
    argparse::ArgumentParser parser("executor");

    parser
        .add_argument("-c", "--cpu-time-limit")
        .required()
        .help("cpu tl in seconds")
        .scan<'i', uint64_t>();

    parser
        .add_argument("-e", "--execute")
        .required()
        .help("string to execute, for example: '/usr/bin/ls -a'");

    parser
        .add_argument("-o", "--output-report-path")
        .default_value("report.json")
        .help("path to output report file (json format)");

    parser.parse_args(argc, argv);

    std::string execute = parser.get<std::string>("--execute");
    uint64_t cpuTimeLimit = parser.get<uint64_t>("--cpu-time-limit");
    std::string outputReportFile = parser.get<std::string>("--output-report-path");

    NDTS::NExecutor::TExecutor executor;

    NDTS::NExecutor::TExecutorArgs args = 
    {
        .execute = std::move(execute),
        .cpuTimeLimitSeconds = cpuTimeLimit,
        .outputReportFile = std::move(outputReportFile)
    };

    executor.Execute(args);
}
