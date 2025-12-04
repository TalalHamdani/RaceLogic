#pragma once

#include <vector>
#include <list>
#include <functional>
#include <optional>
#include <stdexcept>

template<typename K, typename V>
class HashMap {
public:
    HashMap(size_t initial_capacity = 16) 
        : capacity_(initial_capacity), size_(0), load_factor_(0.75) {
        buckets_.resize(capacity_);
    }

    void insert(const K& key, const V& value) {
        if (static_cast<double>(size_) / capacity_ >= load_factor_) {
            rehash();
        }

        size_t index = hash(key);
        auto& bucket = buckets_[index];

        for (auto& pair : bucket) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }

        bucket.push_back({key, value});
        size_++;
    }

    std::optional<V> get(const K& key) const {
        size_t index = hash(key);
        const auto& bucket = buckets_[index];

        for (const auto& pair : bucket) {
            if (pair.first == key) {
                return pair.second;
            }
        }

        return std::nullopt;
    }

    bool contains(const K& key) const {
        return get(key).has_value();
    }

    bool remove(const K& key) {
        size_t index = hash(key);
        auto& bucket = buckets_[index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                size_--;
                return true;
            }
        }

        return false;
    }

    size_t size() const {
        return size_;
    }

    bool isEmpty() const {
        return size_ == 0;
    }

    void clear() {
        for (auto& bucket : buckets_) {
            bucket.clear();
        }
        size_ = 0;
    }

    std::vector<K> keys() const {
        std::vector<K> result;
        result.reserve(size_);
        for (const auto& bucket : buckets_) {
            for (const auto& pair : bucket) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    std::vector<V> values() const {
        std::vector<V> result;
        result.reserve(size_);
        for (const auto& bucket : buckets_) {
            for (const auto& pair : bucket) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

private:
    using Bucket = std::list<std::pair<K, V>>;
    std::vector<Bucket> buckets_;
    size_t capacity_;
    size_t size_;
    double load_factor_;

    size_t hash(const K& key) const {
        return std::hash<K>{}(key) % capacity_;
    }

    void rehash() {
        size_t new_capacity = capacity_ * 2;
        std::vector<Bucket> new_buckets(new_capacity);

        for (auto& bucket : buckets_) {
            for (auto& pair : bucket) {
                size_t new_index = std::hash<K>{}(pair.first) % new_capacity;
                new_buckets[new_index].push_back(std::move(pair));
            }
        }

        buckets_ = std::move(new_buckets);
        capacity_ = new_capacity;
    }
};
