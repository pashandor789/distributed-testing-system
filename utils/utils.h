#pragma once

#include <string>
#include <vector>

#include <boost/outcome.hpp>

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
