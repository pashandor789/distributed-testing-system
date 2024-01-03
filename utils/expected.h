#pragma once

#include <variant>

template <typename TError>
class TUnexpected {
public:
    TUnexpected(TError value)
        : value_(std::move(value))
    {}

    TError value_;
};

template <typename TValue, typename TError>
class TExpected {
public:
    TExpected(TValue val)
        : variant_(std::move(val))
    {}

    TExpected(TUnexpected<TError> unexpected)
        : variant_(std::move(unexpected.value_))
    {}

    TValue Value() {
        return std::get<TValue>(variant_);
    }

    TError Error() {
        return std::get<TError>(variant_);
    }

    bool HasError() {
        return std::holds_alternative<TError>(variant_);
    }

private:
    std::variant<TValue, TError> variant_;
};
