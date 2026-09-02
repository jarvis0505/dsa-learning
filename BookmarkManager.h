#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <string>
#include <iostream>
#include <chrono>
#include <cstdint>
#include "HashTable.h"

class BookmarkManager {
private:
    HashTable<std::string, int> table;
    uint64_t lastStepCount{0};
    double lastExecutionMicros{0.0};

public:
    BookmarkManager() = default;
    ~BookmarkManager() = default;

    bool addBookmark(const std::string& name, int lineNumber);
    int jumpTo(const std::string& name);
    bool removeBookmark(const std::string& name);
    bool contains(const std::string& name) const;
    void clear();

    uint64_t getLastStepCount() const;
    double getLastExecutionMicros() const;
};

#endif // BOOKMARKMANAGER_H
