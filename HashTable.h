#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <string>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include "LinkedList.h"

template <typename KeyType>
struct KeyHasher {
    size_t operator()(const KeyType& key) const {
        return std::hash<KeyType>{}(key);
    }
};

template <>
struct KeyHasher<std::string> {
    size_t operator()(const std::string& key) const {
        size_t hash = 14695981039346656037ULL;
        for (char c : key) {
            hash ^= static_cast<size_t>(c);
            hash *= 1099511628211ULL;
        }
        return hash;
    }
};

template <typename K, typename V>
class HashTable {
public:
    struct KeyValuePair {
        K key;
        V value;
        KeyValuePair(const K& k, const V& v) : key(k), value(v) {}
        bool operator==(const KeyValuePair& other) const {
            return key == other.key;
        }
    };

private:
    LinkedList<KeyValuePair>** buckets{nullptr};
    int bucketCount{16};
    int size{0};
    const double maxLoadFactor{0.75};

    mutable uint64_t lastStepCount{0};
    mutable double lastExecutionMicros{0.0};

    size_t getBucketIndex(const K& key, int cap) const {
        KeyHasher<K> hasher;
        return hasher(key) % static_cast<size_t>(cap);
    }

    void rehash(int newCapacity) {
        LinkedList<KeyValuePair>** newBuckets = new LinkedList<KeyValuePair>*[newCapacity]();
        for (int i = 0; i < newCapacity; ++i) {
            newBuckets[i] = nullptr;
        }

        for (int i = 0; i < bucketCount; ++i) {
            if (buckets[i] != nullptr) {
                for (const auto& kvp : *buckets[i]) {
                    size_t newIdx = getBucketIndex(kvp.key, newCapacity);
                    if (newBuckets[newIdx] == nullptr) {
                        newBuckets[newIdx] = new LinkedList<KeyValuePair>();
                    }
                    newBuckets[newIdx]->push_back(kvp);
                }
                delete buckets[i];
            }
        }
        delete[] buckets;
        buckets = newBuckets;
        bucketCount = newCapacity;
    }

    void allocateBuckets(int cap) {
        bucketCount = cap;
        buckets = new LinkedList<KeyValuePair>*[bucketCount]();
        for (int i = 0; i < bucketCount; ++i) {
            buckets[i] = nullptr;
        }
    }

    void copyFrom(const HashTable<K, V>& other) {
        allocateBuckets(other.bucketCount);
        size = other.size;
        for (int i = 0; i < other.bucketCount; ++i) {
            if (other.buckets[i] != nullptr) {
                buckets[i] = new LinkedList<KeyValuePair>(*other.buckets[i]);
            }
        }
    }

public:
    HashTable(int initialCapacity = 16) {
        allocateBuckets(initialCapacity > 0 ? initialCapacity : 16);
    }

    // Rule of Three: Destructor
    ~HashTable() {
        clear();
        delete[] buckets;
        buckets = nullptr;
    }

    // Rule of Three: Copy Constructor
    HashTable(const HashTable<K, V>& other) {
        copyFrom(other);
    }

    // Rule of Three: Copy Assignment Operator
    HashTable<K, V>& operator=(const HashTable<K, V>& other) {
        if (this != &other) {
            clear();
            delete[] buckets;
            copyFrom(other);
        }
        return *this;
    }

    bool insert(const K& key, const V& value, bool allowOverwrite = false) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        if ((double)(size + 1) / bucketCount > maxLoadFactor) {
            rehash(bucketCount * 2);
            lastStepCount += bucketCount;
        }

        size_t idx = getBucketIndex(key, bucketCount);
        lastStepCount++;

        if (buckets[idx] == nullptr) {
            buckets[idx] = new LinkedList<KeyValuePair>();
            lastStepCount++;
        }

        for (auto& kvp : *buckets[idx]) {
            lastStepCount++;
            if (kvp.key == key) {
                if (allowOverwrite) {
                    kvp.value = value;
                    auto end = std::chrono::high_resolution_clock::now();
                    lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
                    return true;
                } else {
                    auto end = std::chrono::high_resolution_clock::now();
                    lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
                    return false; // Duplicate rejected
                }
            }
        }

        buckets[idx]->push_back(KeyValuePair(key, value));
        size++;
        lastStepCount++;

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return true;
    }

    V* find(const K& key) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        size_t idx = getBucketIndex(key, bucketCount);
        lastStepCount++;

        if (buckets[idx] != nullptr) {
            for (auto& kvp : *buckets[idx]) {
                lastStepCount++;
                if (kvp.key == key) {
                    auto end = std::chrono::high_resolution_clock::now();
                    lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
                    return &kvp.value;
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return nullptr;
    }

    const V* find(const K& key) const {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        size_t idx = getBucketIndex(key, bucketCount);
        lastStepCount++;

        if (buckets[idx] != nullptr) {
            for (const auto& kvp : *buckets[idx]) {
                lastStepCount++;
                if (kvp.key == key) {
                    auto end = std::chrono::high_resolution_clock::now();
                    lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
                    return &kvp.value;
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return nullptr;
    }

    bool contains(const K& key) const {
        return find(key) != nullptr;
    }

    bool remove(const K& key) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        size_t idx = getBucketIndex(key, bucketCount);
        lastStepCount++;

        if (buckets[idx] != nullptr) {
            int indexInList = 0;
            for (const auto& kvp : *buckets[idx]) {
                lastStepCount++;
                if (kvp.key == key) {
                    buckets[idx]->removeAt(indexInList);
                    size--;
                    auto end = std::chrono::high_resolution_clock::now();
                    lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
                    return true;
                }
                indexInList++;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return false;
    }

    int getSize() const {
        return size;
    }

    int getBucketCount() const {
        return bucketCount;
    }

    void clear() {
        if (buckets != nullptr) {
            for (int i = 0; i < bucketCount; ++i) {
                if (buckets[i] != nullptr) {
                    delete buckets[i];
                    buckets[i] = nullptr;
                }
            }
        }
        size = 0;
    }

    uint64_t getLastStepCount() const {
        return lastStepCount;
    }

    double getLastExecutionMicros() const {
        return lastExecutionMicros;
    }

    // Helper to get all keys
    LinkedList<K> getKeys() const {
        LinkedList<K> keys;
        for (int i = 0; i < bucketCount; ++i) {
            if (buckets[i] != nullptr) {
                for (const auto& kvp : *buckets[i]) {
                    keys.push_back(kvp.key);
                }
            }
        }
        return keys;
    }
};

#endif // HASHTABLE_H
