#pragma once

#include <mutex>
#include <optional>
#include <unordered_map>
#include <list>
#include <iostream>

template <typename TKey, typename TValue, size_t capacity>
class TLRUCache {
public:
    void Insert(const TKey& key, const TValue& value) {
        std::lock_guard lock{mtx_};

        if (values_.contains(key)) {
            return;
        }

        order_.push_front({key, value});
        values_[key] = order_.begin();

        if (values_.size() > capacity) {
            auto& [k, v] = order_.back();
            values_.erase(k);
            order_.pop_back();
        }
    }

    std::optional<TValue> Get(const TKey& key) {
        std::lock_guard lock{mtx_};

        auto it = values_.find(key);

        if (it == values_.end()) {
            std::cout << "Cachemiss!" << std::endl;
            return std::nullopt;
        }

        TValue value = std::move(it->second->second);
        order_.erase(it->second);
        order_.push_front({key, value});
        values_[key] = order_.begin();

        std::cout << "Cache works!" << std::endl;

        return value;
    }

    void Erase(const TKey& key) {
        std::lock_guard lock{mtx_};

        if (values_.contains(key)) {
            order_.erase(values_[key]);
            values_.erase(key);
        }
    }

private:
    std::mutex mtx_;

    using TPair = std::pair<TKey, TValue>;
    std::list<TPair> order_{};
    std::unordered_map<TKey, typename std::list<TPair>::iterator> values_{};
};
