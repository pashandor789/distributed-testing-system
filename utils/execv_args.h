#pragma once

#include <string>
#include <vector>

class TExecVArgs {
public:
    TExecVArgs(std::string pathName, std::vector<std::string> args)
        : pathName_(std::move(pathName)) 
        , args_(std::move(args))
    {
        argV_.reserve(args_.size() + 1);

        for (auto& arg: args_) {
            arg.push_back('\0'); //  data is not null-terminated before C++20
            argV_.push_back(arg.data());
        }

        argV_.push_back(nullptr);
    }

    const char* GetPathName() {
        return pathName_.c_str();
    }

    char* const* GetArgV() {
        return const_cast<char* const*>(argV_.data());
    }

private:
    std::string pathName_;
    std::vector<std::string> args_;
private:
    std::vector<char*> argV_;
};
