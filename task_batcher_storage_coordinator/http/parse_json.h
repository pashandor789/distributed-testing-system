#include "utils/utils.h"
#include "utils/expected.h"

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <variant>

std::vector<std::string> FindMissingJSONFields(
    const std::vector<std::string>& expectedFields,
    const nlohmann::json& jsonData
) {
    std::vector<std::string> missingFields;
    for (const auto& fieldName: expectedFields) {
        if (!jsonData.contains(fieldName)) {
            missingFields.push_back(fieldName);
        }
    }
    return missingFields;
}

using JSONParseResult = TExpected<nlohmann::json, std::string>;

JSONParseResult ParseJSON(
    const std::string& serializedJSON,
    const std::vector<std::string>& expectedFields
) {
    nlohmann::json data = nlohmann::json::parse(serializedJSON, nullptr, false);

    if (data.is_discarded()) {
        return std::string("bad json");
    }

    auto missingFields = FindMissingJSONFields(expectedFields, data);

    if (!missingFields.empty()) {
        return std::string("field's weren't specified: " + Join(std::move(missingFields), ","));
    }

    return data;
}
