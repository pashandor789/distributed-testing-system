#include "parse_json.h"

std::string Join(std::vector<std::string> data, const std::string& separator) {
    auto beginData = data.begin();
    auto endData = data.end();

    std::string joinedString;

    if (beginData != endData) {
        joinedString.append(*beginData++);
    }

    while (beginData != endData) {
        joinedString.append(separator).append(*beginData++);
    }

    return joinedString;
}

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

TExpected<nlohmann::json, std::string> ParseJSON(
    const std::string& serializedJSON,
    const std::vector<std::string>& expectedFields
) {
    nlohmann::json data = nlohmann::json::parse(serializedJSON, nullptr, false);

    if (data.is_discarded()) {
        return TUnexpected<std::string>("bad json");
    }

    auto missingFields = FindMissingJSONFields(expectedFields, data);

    if (!missingFields.empty()) {
        return TUnexpected("field's weren't specified: " + Join(std::move(missingFields), ","));
    }

    return data;
}

bool ValidateJSON(
    const nlohmann::json& data,
    const std::vector<std::string>& expectedFields
) {
    return FindMissingJSONFields(data, expectedFields).empty();
}
