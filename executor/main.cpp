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
        .add_argument("-e", "--executable-path")
        .required()
        .help("path to executable file");

    parser
        .add_argument("-o", "--output-report-path")
        .default_value("report.json")
        .help("path to output report file (json format)");

    parser.parse_args(argc, argv);

    std::string executablePath = parser.get<std::string>("--executable-path");
    uint64_t cpuTimeLimit = parser.get<uint64_t>("--cpu-time-limit");

    NDTS::NExecutor::TExecutor executor;

    NDTS::NExecutor::TExecutorArgs args = 
    {
        .executablePath = std::move(executablePath),
        .cpuTimeLimitSeconds = cpuTimeLimit
    };

    std::string outputReportPath = parser.get<std::string>("--output-report-path");

    executor.Execute(args, outputReportPath);
}