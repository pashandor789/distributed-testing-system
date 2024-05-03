#include "storage_client.h"

#include "utils/lru_cache.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace NDTS::NTabasco {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

static mongocxx::instance instance{};

/*

    schema: (collection)
        task:
            taskId: ui64
            testId: ui64
            testType: input/output (string)
            content: string
        task_meta:
            taskId: ui64
            batches: vector<ui64>
            batchSize: ui64
        builds:
            id: ui64
            executeScript: string
            initScript: string

*/

class TStorageClient::TImpl {
public:
    TImpl(const TStorageClientConfig& config)
        : client_(mongocxx::uri(config.uri()))
        , db_(client_.database(config.dbname()))
        , buildsCollection_(db_.collection("builds"))
        , taskMetaCollection_(db_.collection("taskMeta"))
        , taskTestsCollection_(db_.collection("taskTests"))
    {}

    TOptionalError UpsertTaskTests(TTask task) {
        try {
            size_t testCount = task.tests.inputTests.size();

            std::vector<bsoncxx::document::value> documents;
            documents.reserve(testCount * 2);

            auto& inputTests = task.tests.inputTests;

            for (size_t testIndex = 0; testIndex < testCount; ++testIndex) {
                auto options = mongocxx::options::update().upsert(true);

                auto upsertDoc = make_document(
                    kvp("$set", make_document(kvp("content", std::move(inputTests[testIndex]))))
                );

                auto filter = make_document(
                    kvp("taskId", static_cast<int64_t>(task.id)),
                    kvp("testId", static_cast<int64_t>(testIndex + 1)),
                    kvp("testType", "input")
                );

                taskTestsCollection_.update_one(filter.view(), upsertDoc.view(), options);
            }

            auto& outputTests = task.tests.outputTests;

            for (size_t testIndex = 0; testIndex < testCount; ++testIndex) {
                auto options = mongocxx::options::update().upsert(true);

                auto upsertDoc = make_document(
                    kvp("$set", make_document(kvp("content", std::move(outputTests[testIndex]))))
                );

                auto filter = make_document(
                    kvp("taskId", static_cast<int64_t>(task.id)),
                    kvp("testId", static_cast<int64_t>(testIndex + 1)),
                    kvp("testType", "output")
                );

                taskTestsCollection_.update_one(filter.view(), upsertDoc.view(), options);
            }
            return std::nullopt;
        } catch (std::exception& e) {
            return e.what();
        }
    }

    TOptionalError UpsertTaskMetaData(TTaskMetaData taskMetaData) {
        try {
            uint64_t taskId = taskMetaData.taskId;

            auto upsertDoc = make_document(
                kvp(
                    "$set",
                    bsoncxx::from_json(taskMetaData.MoveToJSON().dump())
                )
            );

            auto filter = make_document(kvp("_id", static_cast<int64_t>(taskMetaData.taskId)));

            auto options = mongocxx::options::update().upsert(true);

            taskMetaCollection_.update_one(filter.view(), upsertDoc.view(), options);

//            taskMetaCache_.Erase(taskId);

            return std::nullopt;
        } catch (std::exception& e) {
            return e.what();
        }
    }

    TExpected<TTaskMetaData, TError> GetTaskMetaData(uint64_t taskId) {
        try {
//            if (auto maybeMeta = taskMetaCache_.Get(taskId)) {
//                return maybeMeta.value();
//            }

            auto maybeMeta = taskMetaCollection_.find_one(make_document(kvp("_id", static_cast<int64_t>(taskId))));

            if (!maybeMeta.has_value()) {
                auto err = TError{
                    .msg = "404 Not Found TaskMetaData for id: " + std::to_string(taskId),
                    .errorCode = NOT_FOUND
                };

                return TUnexpected(std::move(err));
            }

            auto metaDoc = std::move(maybeMeta.value());
            TTaskMetaData metaData =
                TTaskMetaData::FromJSON(nlohmann::json::parse(bsoncxx::to_json(metaDoc.view())));

//            taskMetaCache_.Insert(taskId, metaData);

            return metaData;
        } catch (std::exception& e) {
            return TUnexpected<TError>({.msg = e.what()});
        }
    }

    TExpected<TTaskTests, TError> GetTaskBatch(uint64_t taskId, uint64_t batchId) {
        try {
            auto getResp = GetTaskMetaData(taskId);

            if (getResp.HasError()) {
                return TUnexpected(getResp.Error());
            }

            auto taskMetaData = std::move(getResp.Value());

            TTaskTests tests;
            auto& inputTests = tests.inputTests;
            auto& outputTests = tests.outputTests;

            int64_t frontTestId = taskMetaData.batches[batchId].front();
            int64_t backTestId = taskMetaData.batches[batchId].back();

            int64_t batchTestCount = backTestId - frontTestId + 1;

            inputTests.resize(batchTestCount);
            outputTests.resize(batchTestCount);

            auto filter = make_document(
                kvp("taskId", static_cast<int64_t>(taskId)),
                kvp("testId",
                    make_document(
                        kvp("$gte", frontTestId),
                        kvp("$lte", backTestId)
                    )
                )
            );

            for (auto& doc: taskTestsCollection_.find(filter.view())) {
                auto testType = doc["testType"].get_string().value;
                auto testId = doc["testId"].get_int64().value;
                auto content = doc["content"].get_string().value;

                if (testType == "input") {
                   inputTests[testId - frontTestId] = content;
                }

                if (testType == "output") {
                    outputTests[testId - frontTestId] = content;
                }
            }

            return tests;
        } catch (std::exception& e) {
            return TUnexpected<TError>({.msg = e.what()});
        }
    }

    TOptionalError UpsertBuild(TBuild build) {
        try {
            uint64_t buildId = build.id;
            auto jsonBuild = build.MoveToJSON();
            jsonBuild.erase("id");

            auto upsertDoc = make_document(
                kvp(
                    "$set",
                    bsoncxx::from_json(jsonBuild.dump())
                )
            );

            auto filter = make_document(kvp("_id", static_cast<int64_t>(build.id)));
            auto options = mongocxx::options::update().upsert(true);
            buildsCollection_.update_one(filter.view(), upsertDoc.view(), options);

//            buildsCache_.Erase(buildId);

            return std::nullopt;
        } catch (std::exception& e) {
            return e.what();
        }
    }

    TExpected<TBuild, TError> GetBuild(uint64_t buildId) {
        try {
//            if (auto maybeBuild = buildsCache_.Get(buildId)) {
//                return maybeBuild.value();
//            }

            auto maybeBuild = buildsCollection_.find_one(make_document(kvp("_id", static_cast<int64_t>(buildId))));

            if (!maybeBuild.has_value()) {
                auto err = TError{
                    .msg = "404 Not Found Build for id: " + std::to_string(buildId),
                    .errorCode = NOT_FOUND
                };

                return TUnexpected(std::move(err));
            }

            auto buildDoc = std::move(maybeBuild.value());
            TBuild build = TBuild::FromJSON(nlohmann::json::parse(bsoncxx::to_json(buildDoc)));

//            buildsCache_.Insert(buildId, build);

            return build;
        } catch (std::exception& e) {
            return TUnexpected<TError>({.msg = e.what()});
        }
    }

    TExpected<TBuilds, TError> GetBuilds() {
       try {
           TBuilds builds;

           for (const auto& buildDoc: buildsCollection_.find({})) {
               builds.items.push_back(TBuild::FromJSON(nlohmann::json::parse(bsoncxx::to_json(buildDoc))));
           }

           return builds;
       } catch (std::exception& e) {
           return TUnexpected<TError>({.msg = e.what()});
       }
    }

private:
//    TLRUCache<uint64_t, TBuild, 3> buildsCache_{};
//    TLRUCache<uint64_t, TTaskMetaData, 50> taskMetaCache_{};

private:
    mongocxx::client client_;
    mongocxx::database db_;

private:
    mongocxx::collection buildsCollection_;
    mongocxx::collection taskMetaCollection_;
    mongocxx::collection taskTestsCollection_;
};

TStorageClient::TStorageClient(const TStorageClientConfig& config)
    : pImpl_(std::make_shared<TImpl>(config))
{}

TStorageClient::TOptionalError TStorageClient::UpsertTaskTests(TTask task) {
    return pImpl_->UpsertTaskTests(std::move(task));
}

TStorageClient::TOptionalError TStorageClient::UpsertTaskMetaData(TTaskMetaData taskMetaData) {
    return pImpl_->UpsertTaskMetaData(std::move(taskMetaData));
}

TStorageClient::TOptionalError TStorageClient::UpsertBuild(TBuild build) {
    return pImpl_->UpsertBuild(std::move(build));
}

TExpected<TTaskTests, TStorageClient::TError> TStorageClient::GetTaskTestsBatch(uint64_t taskId, uint64_t batchId) {
    return pImpl_->GetTaskBatch(taskId, batchId);
}

TExpected<TTaskMetaData, TStorageClient::TError> TStorageClient::GetTaskMetaData(uint64_t taskId) {
    return pImpl_->GetTaskMetaData(taskId);
}

TExpected<TBuild, TStorageClient::TError> TStorageClient::GetBuild(uint64_t buildId) {
    return pImpl_->GetBuild(buildId);
}

TExpected<TBuilds, TStorageClient::TError> TStorageClient::GetBuilds() {
    return pImpl_->GetBuilds();
}

} // end of NDTS::NTabasco namespace
