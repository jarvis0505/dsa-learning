#include "BookmarkManager.h"

using namespace std;

bool BookmarkManager::addBookmark(const string& name, int lineNumber) {
    auto start = chrono::high_resolution_clock::now();
    
    bool inserted = table.insert(name, lineNumber, false);
    
    lastStepCount = table.getLastStepCount();
    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return inserted;
}

int BookmarkManager::jumpTo(const string& name) {
    auto start = chrono::high_resolution_clock::now();
    
    int* valPtr = table.find(name);
    lastStepCount = table.getLastStepCount();
    
    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();

    if (valPtr != nullptr) {
        return *valPtr;
    }
    return -1;
}

bool BookmarkManager::removeBookmark(const string& name) {
    auto start = chrono::high_resolution_clock::now();
    
    bool removed = table.remove(name);
    lastStepCount = table.getLastStepCount();
    
    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return removed;
}

bool BookmarkManager::contains(const string& name) const {
    return table.contains(name);
}

void BookmarkManager::clear() {
    table.clear();
}

uint64_t BookmarkManager::getLastStepCount() const {
    return lastStepCount;
}

double BookmarkManager::getLastExecutionMicros() const {
    return lastExecutionMicros;
}
