#pragma once

#include <variant>

template <typename TError>
class TUnexpected {
public:
    explicit TUnexpected(TError value)
        : value_(std::move(value))
    {}

    TError value_;
};

template <typename TValue, typename TError>
class TExpected {
public:
    TExpected(TValue val) {
        new (&value_) TValue(std::move(val));
    }

    TExpected(TUnexpected<TError> unexpected)
        : hasError_(true)
    {
        new (&value_) TValue(std::move(unexpected.value_));
    }

    TValue& Value() {
        return *reinterpret_cast<TValue*>(&value_);
    }

    TError& Error() {
        assert(hasError_);
        return *reinterpret_cast<TError*>(&value_);
    }

    bool HasError() {
        return hasError_;
    }

    ~TExpected() {
        if (hasError_) {
            reinterpret_cast<TError*>(&error_)->~TError();
        } else {
            reinterpret_cast<TValue*>(&value_)->~TValue();
        }
    }

private:
    std::aligned_storage_t<sizeof(TValue), alignof(TValue)> value_;
    std::aligned_storage_t<sizeof(TError), alignof(TError)> error_;
    bool hasError_ = false;
};
