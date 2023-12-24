#include "testing_processor.h"
#include "tabasco_request_task.h"

#include <nlohmann/json.hpp>

#include <filesystem>

namespace NDTS::NTestingProcessor {

namespace fs = std::filesystem;

static const fs::path USER_ROOT_PATH = "/check";
static const fs::path INIT_SCRIPT_PATH = USER_ROOT_PATH / "init.sh";
static const fs::path EXECUTE_SCRIPT_PATH = USER_ROOT_PATH / "execute.sh";
static const fs::path USER_DATA_PATH = USER_ROOT_PATH / "userData";
static const fs::path USER_EXECUTABLE_PATH = USER_ROOT_PATH / "executable";
static const fs::path CHECKER_PATH = "diff";

enum class TVerdict {
    OK = 0,
    WA = 1,
    TL = 2,
    ML = 3,
    RE = 4,
    CRASH = 5
};

struct TTestingReport {
    TTestingReport(uint64_t cpuTimeElapsedMicroSeconds, uint64_t memorySpent)
        : cpuTimeElapsedMicroSeconds(cpuTimeElapsedMicroSeconds)
        , memorySpent(memorySpent)
        , verdict(TVerdict::OK)
    {}

    TVerdict verdict;
    uint64_t cpuTimeElapsedMicroSeconds;
    uint64_t memorySpent;
};

TTestingProcessor::TTestingProcessor(const TTestingProcessorConfig& config)
    : container_(config.docker_container_config())
    , tabasco_(NTabasco::TTabascoGRPC::NewStub(grpc::CreateChannel(config.tabasco_url(), grpc::InsecureChannelCredentials())))
{}

void TTestingProcessor::Process(TTestingProcessorRequest request) {
    container_.Run();

    uint64_t batchCount = 0;
    Prepare(request, &batchCount);
    auto report = Test(request, batchCount);
    Commit(std::move(report));

    container_.Kill();
}

bool TTestingProcessor::Prepare(TTestingProcessorRequest& request, uint64_t* const batchCount) {
    TTabascoRequestTask tabascoRequestTask(tabasco_);

    TGetScriptsResponse scripts = tabascoRequestTask.GetScripts(request.taskId, request.buildId);

    container_.CreateFile(INIT_SCRIPT_PATH, std::move(scripts.initScript));
    container_.CreateFile(USER_DATA_PATH, std::move(request.userData));
    *batchCount = scripts.batchCount;

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
    const fs::path& checkerPath,
    const fs::path& outputTestPath,
    const fs::path& userOutputPath,
    const fs::path& inputTestPath
) {
    std::string checkerLauncherCommand;

    checkerLauncherCommand
        .append(checkerPath)
        .append(" ")
        .append(userOutputPath)
        .append(" ")
        .append(outputTestPath);

    int checkerExitCode = system(checkerLauncherCommand.c_str());

    if (checkerExitCode == 0) {
        return TVerdict::OK;
    }

    if (checkerExitCode == static_cast<int>(TVerdict::WA)) {
        return TVerdict::WA;
    }

    return TVerdict::CRASH;
}

void DumpTests(
    const fs::path& inputTestPath,
    const std::string& inputTest,
    const fs::path& outputTestPath,
    const std::string& outputTest
) {
    std::ofstream inputTestFile(inputTestPath);
    inputTestFile << inputTest;

    std::ofstream outputTestFile(outputTestPath);
    outputTestFile << outputTest;
}

std::vector<TTestingReport> TTestingProcessor::Test(TTestingProcessorRequest& request, uint64_t batchCount) {
    TTabascoRequestTask tabascoRequestTask(tabasco_);

    std::vector<TTestingReport> report;

    size_t testIndex = 1;

    for (size_t batchIndex = 0; batchIndex < batchCount; ++batchIndex) {
        TGetBatchResponse tests = tabascoRequestTask.GetBatch(request.taskId, batchIndex);
        size_t testsSize = tests.inputTests.size();

        for (size_t i = 0; i < testsSize; ++i) {
            fs::path inputTestPath = localStoragePath_ / "input.txt";
            fs::path outputTestPath = localStoragePath_ / "output.txt";
            fs::path userOutputPath = localStoragePath_ / "userOutput.txt";

            DumpTests(inputTestPath, tests.inputTests[i], outputTestPath, tests.outputTests[i]);

            container_.Exec(
                {EXECUTE_SCRIPT_PATH, USER_EXECUTABLE_PATH},
                inputTestPath,
                userOutputPath
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

            TVerdict checkerVerdict = CheckOutput(CHECKER_PATH, outputTestPath, userOutputPath, inputTestPath);

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
            "commited test #%ld %ld %ld %d",
            i + 1,
            report[i].cpuTimeElapsedMicroSeconds,
            report[i].memorySpent,
            static_cast<int>(report[i].verdict)
        );
    }
}

} // end of NDTS::NTestingProcessor namespace