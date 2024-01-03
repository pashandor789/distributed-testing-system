#include "utils/expected.h"

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <variant>

std::string Join(std::vector<std::string> data, const std::string& separator = " ");

std::vector<std::string> FindMissingJSONFields(
    const std::vector<std::string>& expectedFields,
    const nlohmann::json& jsonData
);

using JSONParseResult = TExpected<nlohmann::json, std::string>;

JSONParseResult ParseJSON(
    const std::string& serializedJSON,
    const std::vector<std::string>& expectedFields
);
