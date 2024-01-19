#include "expected.h"

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <variant>

// on error return's string of missing fields separated with ','
TExpected<nlohmann::json, std::string> ParseJSON(
    const std::string& serializedJSON,
    const std::vector<std::string>& expectedFields
);

bool ValidateJSON(
    const nlohmann::json& data,
    const std::vector<std::string>& expectedFields
);
