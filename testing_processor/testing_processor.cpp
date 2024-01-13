#include "commit_service_request.h"
#include "testing_processor.h"
#include "testing_report.h"
#include "tabasco_request_task.h"

#include <nlohmann/json.hpp>

#include <glog/logging.h>

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

TTestingProcessor::TTestingProcessor(const TTestingProcessorConfig& config)
    : container_(config.docker_container_config())
    , tabasco_(NTabasco::TTabascoGRPC::NewStub(grpc::CreateChannel(config.tabasco_url(), grpc::InsecureChannelCredentials())))
    , localStoragePath_(config.local_storage_path())
    , commitServiceURL_(config.commit_service_url())
{}

void TTestingProcessor::Process(TTestingProcessorRequest request) {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    container_.Run();

    Prepare(request);
    auto report = Test(request);
    Commit(request, std::move(report));

    container_.Kill();

    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

bool TTestingProcessor::Prepare(TTestingProcessorRequest& request) {
    TTabascoRequestTask tabascoRequestTask(tabasco_);

    auto getScriptsResponse = tabascoRequestTask.GetScripts(request.taskId, request.buildName);

    if (getScriptsResponse.HasError()) {
        LOG(ERROR) << "TabascoRequestTask failed: " <<  getScriptsResponse.Error().msg << " for submission: " << request.submissionId;
        return false;
    }

    TGetScriptsResponse response = std::move(getScriptsResponse.Value());

    container_.CreateFile(INIT_SCRIPT_PATH, response.initScript);
    container_.Exec({"chmod", "+x", INIT_SCRIPT_PATH});

    container_.CreateFile(USER_DATA_PATH, request.userData);
    request.batchCount = response.batchCount;

    int exitCode = container_.ExecBash(
        {INIT_SCRIPT_FILE, USER_DATA_PATH},
        {.workingDir = USER_ROOT_PATH}
    );

    if (exitCode != 0) {
        return false;
    }

    container_.CreateFile(EXECUTE_SCRIPT_PATH, response.executeScript);
    container_.Exec({"chmod", "+x", EXECUTE_SCRIPT_PATH});

    if (!response.rootDir.empty()) {
        container_.CreateFile(USER_ROOT_PATH / "root_dir.zip", response.rootDir);
        container_.Exec({"unzip", "root_dir.zip"}, {.workingDir = USER_ROOT_PATH});
    }

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

    if (request.batchCount == 0) {
        return { ExecuteAndTest(request, "", "") };
    }

    for (size_t batchIndex = 0; batchIndex < request.batchCount; ++batchIndex) {
        auto getBatchResponse = tabascoRequestTask.GetBatch(request.taskId, batchIndex);

        if (getBatchResponse.HasError()) {
            report.emplace_back(0, 0);
            report.back().verdict = TVerdict::CRASH;
            return report;
        }

        TGetBatchResponse tests = std::move(getBatchResponse.Value());
        size_t testsSize = tests.inputTests.size();

        for (size_t i = 0; i < testsSize; ++i) {
            report.push_back(
                ExecuteAndTest(request, tests.inputTests[i], tests.outputTests[i])
            );

            if (report.back().verdict != TVerdict::OK) {
                return report;
            }
        }
    }

    return report;
}

TTestingReport TTestingProcessor::ExecuteAndTest(
    TTestingProcessorRequest& request,
    const std::string& inputTest,
    const std::string& outputTest
) {
    fs::path inputTestPath = localStoragePath_ / "input.txt";
    fs::path outputTestPath = localStoragePath_ / "output.txt";
    fs::path userOutputPath = localStoragePath_ / "userOutput.txt";

    DumpTests(inputTestPath, inputTest, outputTestPath, outputTest);

    std::string cpuTLMS = std::to_string(request.cpuTimeLimitMilliSeconds);
    container_.Exec(
            {EXECUTOR_SCRIPT_PATH, "--execute", EXECUTE_SCRIPT_PATH, "--cpu-time-limit", std::move(cpuTLMS)},
            {.stdIn = inputTestPath, .stdOut = userOutputPath, .workingDir = USER_ROOT_PATH}
    );

    container_.Exec({"cat", "report.json"}, {.stdOut = localStoragePath_ / "report.json", .workingDir = USER_ROOT_PATH});

    std::string deserializedReport = FetchFileContent(localStoragePath_ / "report.json");
    nlohmann::json executeReport = nlohmann::json::parse(deserializedReport, nullptr, false);

    TTestingReport report(
        static_cast<uint64_t>(executeReport["cpuTimeElapsedMicroSeconds"]) / 1'000,
        executeReport["memorySpent"]
    );

    if (executeReport["exitCode"] != 0) {
        report.verdict = TVerdict::RE;
        return report;
    }

    if (executeReport["cpuTimeElapsedMicroSeconds"] > request.cpuTimeLimitMilliSeconds * 1'000) {
        report.verdict = TVerdict::TL;
        return report;
    }

    if (executeReport["memorySpent"] > request.memoryLimit) {
        report.verdict = TVerdict::ML;
        return report;
    }

    TVerdict checkerVerdict = CheckOutput(CHECKER_PATH, outputTestPath, userOutputPath, inputTestPath);

    if (checkerVerdict != TVerdict::OK) {
        report.verdict = checkerVerdict;
        return report;
    }

    return report;
}


void TTestingProcessor::Commit(TTestingProcessorRequest& request, std::vector<TTestingReport>&& report) {
    TCommitServiceRequest commitServiceRequest(commitServiceURL_);
    auto err = commitServiceRequest.Commit(request.submissionId, report);

    if (err.has_value()) {
        nlohmann::json kek;
        kek["items"] = ToJSON(report);
        std::cerr << kek.dump() << std::endl;
        LOG(ERROR) << "CommitService failed: " << err.value() << " for submission: " << request.submissionId;
    }
}

} // end of NDTS::NTestingProcessor namespace
