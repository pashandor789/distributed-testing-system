#include "testing_processor.h"
#include "tabasco_request_task.h"

#include <nlohmann/json.hpp>

#include <filesystem>

namespace NDTS::NTestingProcessor {

namespace fs = std::filesystem;

static const fs::path USER_ROOT_PATH = "/check";
static const fs::path INIT_SCRIPT_FILE = "init.sh";
static const fs::path INIT_SCRIPT_PATH = USER_ROOT_PATH / INIT_SCRIPT_FILE;
static const fs::path EXECUTE_SCRIPT_FILE = "execute.sh";
static const fs::path EXECUTE_SCRIPT_PATH = USER_ROOT_PATH / EXECUTE_SCRIPT_FILE;
static const fs::path USER_DATA_PATH = USER_ROOT_PATH / "userData";
static const fs::path USER_EXECUTABLE_FILE = "executable";
static const fs::path USER_EXECUTABLE_PATH = USER_ROOT_PATH / USER_EXECUTABLE_FILE;
static const fs::path CHECKER_PATH = "/usr/bin/diff";
static const fs::path EXECUTOR_SCRIPT_FILE = "executor";
static const fs::path EXECUTOR_SCRIPT_PATH = USER_ROOT_PATH / EXECUTOR_SCRIPT_FILE;

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
    , localStoragePath_(config.local_storage_path())
{}

void TTestingProcessor::Process(TTestingProcessorRequest request) {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    container_.Run();

    Prepare(request);
    auto report = Test(request);
    // Commit(std::move(report));

    container_.Kill();

    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << 1.0 * time / 1000  << std::endl;
}

bool TTestingProcessor::Prepare(TTestingProcessorRequest& request) {
    TTabascoRequestTask tabascoRequestTask(tabasco_);

    TGetScriptsResponse scripts = tabascoRequestTask.GetScripts(request.taskId, request.buildId);

    container_.CreateFile(INIT_SCRIPT_PATH, std::move(scripts.initScript));
    container_.Exec({"chmod", "+x", INIT_SCRIPT_PATH});

    container_.CreateFile(USER_DATA_PATH, std::move(request.userData));
    request.batchCount = scripts.batchCount;

    int exitCode = container_.ExecBash(
        {INIT_SCRIPT_FILE, USER_DATA_PATH},
        {.workingDir = USER_ROOT_PATH}
    );

    if (exitCode != 0) {
        return false;
    }

    container_.CreateFile(EXECUTE_SCRIPT_PATH, std::move(scripts.executeScript));
    container_.Exec({"chmod", "+x", EXECUTE_SCRIPT_PATH});
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

std::string FetchFileContent(const fs::path& filePath) {
    std::string fileContent;
    std::ifstream file(filePath);

    file.seekg(0, std::ios::end);
    size_t contentSize = file.tellg();
    fileContent.resize(contentSize, '\0');
    file.seekg(0);
    file.read(&fileContent[0], static_cast<std::streamsize>(contentSize));

    return fileContent;
}

/* 
    TODO:   
        1) Make tests upload async
*/
std::vector<TTestingReport> TTestingProcessor::Test(TTestingProcessorRequest& request) {
    TTabascoRequestTask tabascoRequestTask(tabasco_);

    std::vector<TTestingReport> report;

    for (size_t batchIndex = 0; batchIndex < request.batchCount; ++batchIndex) {
        TGetBatchResponse tests = tabascoRequestTask.GetBatch(request.taskId, batchIndex);
        size_t testsSize = tests.inputTests.size();

        for (size_t i = 0; i < testsSize; ++i) {
            fs::path inputTestPath = localStoragePath_ / "input.txt";
            fs::path outputTestPath = localStoragePath_ / "output.txt";
            fs::path userOutputPath = localStoragePath_ / "userOutput.txt";

            DumpTests(inputTestPath, tests.inputTests[i], outputTestPath, tests.outputTests[i]);

            std::string cpuTLMS = std::to_string(request.cpuTimeLimitMilliSeconds);
            container_.Exec(
                {EXECUTOR_SCRIPT_PATH, "--execute", EXECUTE_SCRIPT_PATH, "--cpu-time-limit", std::move(cpuTLMS)},
                {.stdIn = inputTestPath, .stdOut = userOutputPath, .workingDir = USER_ROOT_PATH}
            );

            container_.Exec({"cat", "report.json"}, {.stdOut = localStoragePath_ / "report.json", .workingDir = USER_ROOT_PATH});

            std::string deserializedReport = FetchFileContent(localStoragePath_ / "report.json");
            nlohmann::json executeReport = nlohmann::json::parse(deserializedReport, nullptr, false);

            report.emplace_back(
                executeReport["cpuTimeElapsedMicroSeconds"],
                executeReport["memorySpent"]
            );

            if (executeReport["exitCode"] != 0) {
                report.back().verdict = TVerdict::RE;
                return report;
            }

            if (executeReport["cpuTimeElapsedMicroSeconds"] > request.cpuTimeLimitMilliSeconds * 1'000) {
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
        }
    }

    return report;
}

void TTestingProcessor::Commit(std::vector<TTestingReport>&& report) {
    for (size_t i = 0; i < report.size(); ++i) {
        printf(
            "commited test #%ld %ld %ld %d\n",
            i + 1,
            report[i].cpuTimeElapsedMicroSeconds,
            report[i].memorySpent,
            static_cast<int>(report[i].verdict)
        );
    }
}

} // end of NDTS::NTestingProcessor namespace
