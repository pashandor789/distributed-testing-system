#include "testing_processor.h"
#include "tabasco_request_task.h"

#include <nlohmann/json.hpp>

#include <filesystem>

namespace NDTS::NTestingProcessor {

namespace fs = std::filesystem;

static const std::fs::path USER_ROOT_PATH = "/check";
static const std::fs::path INIT_SCRIPT_PATH = USER_ROOT_PATH / "init.sh";
static const std::fs::path EXECUTE_SCRIPT_PATH = USER_ROOT_PATH / "execute.sh";
static const std::fs::path USER_DATA_PATH = USER_ROOT_PATH / "userData";
static const std::fs::path USER_EXECUTABLE_PATH = USER_ROOT_PATH / "executable";
static const std::fs::path CHECKER_PATH = "diff";

enum class TVerdict {
    OK = 0,
    WA = 1,
    TL = 2,
    ML = 3,
    RE = 4,
    CRASH = 5
}

struct TTestingReport {
    TTestingReport(uint64_t cpuTimeElapsedMicroSeconds, uint64_t memorySpent)
        : cpuTimeElapsedMicroSeconds(cpuTimeElapsedMicroSeconds)
        , memorySpent(memorySpent)
        , verdict(TVerdict::OK)
    {}

    TVerdict verdict;
    uint64_t cpuTimeElapsedMicroSeconds = 0;
    uint64_t memorySpent = 0;
}

TTestingProcessor::TTestingProcessor(const TTestingProcessorConfig& config)
    : container_(config.docker_container_config())
{}

TTestingProcessor::Process(TTestingProcessorRequest request) {
    container_.Run();

    Prepare(request);
    auto report = Test(request);
    Commit(std::move(report));

    container_.Kill();
}

bool TTestingProcessor::Prepare(TTestingProcessorRequest& request, uint64* const batchCount) {
    TTabascoRequestTask tabascoRequestTask;

    TGetScriptsResponse response = tabascoRequestTask.GetScripts(request.taskId, request.buildId);

    container_.CreateFile(INIT_SCRIPT_PATH, std::move(scripts.initScript));
    container_.CreateFile(USER_DATA_PATH, std::move(request.userData));
    *batchCount = response.batchCount;

    int exitCode = container_.Exec(
        {INIT_SCRIPT_PATH, USER_DATA_PATH},
        std::nullopt,
        std::nullopt
    );
    
    if (exitCode != 0) {
        return false;
    }

    container_.CreateFile(EXECUTE_SCRIPT_PATH, std::move(scripts.executeScript));
    return true;
}

TVerdict CheckOutput(
    const std::fs::path& checkerPath,
    const std::fs::path& inputTestPath,
    const std::fs::path& outputTestPath
) {
    std::string checkerLauncherCommand;

    checkerLauncherCommand
        .append(checkerPath)
        .append(" ")
        .append(inputTestPath)
        .append(" ")
        .append(outputTestPath)

    int checkerExitCode = system(checkerLauncherCommand);

    if (checkerExitCode == 0) {
        return TVerdict::OK;
    }

    if (checkerExitCode == TVerdict::WA) {
        return TVerdict::WA;
    }

    return TVerdict::CRASH;
}

void DumpTests(
    const std::fs::path& inputTestPath,
    const std::string& inputTest,
    const std::fs::path& outputTest,
    const std::string& outputTest
) {
    std::ofstream inputTestFile(inputTestPath);
    inputTestFile << inputTest;

    std::ofstream outputTestFile(outputTestPath);
    outputTestFile << outputTest;
}

std::vector<TTestingReport> TTestingProcessor::Test(TTestingProcessorRequest& request, uint64_t batchCount) {
    TTabascoRequestTask tabascoRequestTask;

    std::vector<TTestingReport> report;

    size_t testIndex = 1;

    for (size_t batchIndex = 0; batchIndex < batchCount; ++batchIndex) {
        TGetBatchResponse tests = tabascoRequestTask.GetBatch(request.taskId, batchIndex);
        size_t testsSize = tests.inputTests.size();

        for (size_t i = 0; i < testsSize; ++i) {
            std::fs::path inputTestPath = localStoragePath_ / "input.txt";
            std::fs::path outputTestPath = localStoragePath_ / "output.txt";

            DumpTests(inputTestPath, tests.inputTests[i], outputTestPath, tests.outputTests[i]);

            container_.Exec(
                {EXECUTE_SCRIPT_PATH, USER_EXECUTABLE_PATH},
                inputPath,
                outputPath
            );

            std::string deserializedReport;
            nlohmann::json executeReport = nlohmann::json::parse(deserializedReport, nullptr, false);

            report.emplace_back(
                executeReport["cpuTimeElapsedMicroSeconds"],
                executeReport["memorySpent"]
            );

            if (executeReport["exitCode"] != 0) {
                report.back().verdict = TVerdict::RE;
                return report;
            }

            if (executeReport["cpuTimeElapsedMicroSeconds"] > request.cpuTimeLimitSeconds * 1'000'000) {
                report.back().verdict = TVerdict::TL;
                return report;
            }

            if (executeReport["memorySpent"] > request.memoryLimit) {
                report.back().verdict = TVerdict::ML;
                return report; 
            }

            TVerdict checkerVerdict = CheckOutput(CHECKER_PATH, inputTestPath, outputTestPath);

            if (checkerVerdict != TVerdict::OK) {
                report.back().verdict = checkerVerdict;
                return report;
            }

            ++testIndex;
        }
    }

    return report;
}

void TTestingProcessor::Commit(std::vector<TTestingReport>&& report) {
    for (size_t i = 0; i < report.size(); ++i) {
        printf(
            "test #%ld %ld %ld %ld",
            i + 1,
            report[i].cpuTimeElapsedMicroSeconds,
            report[i].memorySpent,
            report[i].verdict
        );
    }
}

} // end of NDTS::NTestingProcessor namespace