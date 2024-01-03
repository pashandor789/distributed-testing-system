#pragma once

#include <variant>

template <typename TValue, typename TError>
class TExpected {
public:
    TExpected(TValue val)
        : variant_(val)
    {}

    TExpected(TError err)
        : variant_(err)
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
