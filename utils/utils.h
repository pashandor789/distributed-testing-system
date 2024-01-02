#pragma once

#include <string>
#include <vector>

template <typename TJSON>
std::vector<std::string> FindMissingJSONFields(const std::vector<std::string>& expectedFields, const TJSON jsonData) {
    std::vector<std::string> missingFields;
    for (const auto& fieldName: expectedFields) {
        if (!jsonData.contains(fieldName)) {
            missingFields.push_back(fieldName);
        }
    }
    return missingFields;
}

std::string Join(std::vector<std::string> data, const std::string& separator = " ") {
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