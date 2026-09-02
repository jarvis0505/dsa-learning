#include "WordIndex.h"
#include <sstream>

using namespace std;

WordIndex::~WordIndex() {
    clear();
}

void WordIndex::clear() {
    LinkedList<string> keys = idx.getKeys();
    for (const auto& key : keys) {
        LinkedList<int>** listPtrPtr = idx.find(key);
        if (listPtrPtr != nullptr && *listPtrPtr != nullptr) {
            delete *listPtrPtr;
        }
    }
    idx.clear();
    isDirty = true;
}

WordIndex::WordIndex(const WordIndex& other) {
    LinkedList<string> keys = other.idx.getKeys();
    for (const auto& key : keys) {
        LinkedList<int>* const* listPtrPtr = other.idx.find(key);
        if (listPtrPtr != nullptr && *listPtrPtr != nullptr) {
            LinkedList<int>* newList = new LinkedList<int>(**listPtrPtr);
            idx.insert(key, newList, true);
        }
    }
    isDirty = other.isDirty;
}

WordIndex& WordIndex::operator=(const WordIndex& other) {
    if (this != &other) {
        clear();
        LinkedList<string> keys = other.idx.getKeys();
        for (const auto& key : keys) {
            LinkedList<int>* const* listPtrPtr = other.idx.find(key);
            if (listPtrPtr != nullptr && *listPtrPtr != nullptr) {
                LinkedList<int>* newList = new LinkedList<int>(**listPtrPtr);
                idx.insert(key, newList, true);
            }
        }
        isDirty = other.isDirty;
    }
    return *this;
}

string WordIndex::cleanToken(const string& token) const {
    string result;
    for (char ch : token) {
        if (isalnum(static_cast<unsigned char>(ch))) {
            result += static_cast<char>(tolower(static_cast<unsigned char>(ch)));
        }
    }
    return result;
}

void WordIndex::addWord(const string& word, int lineNumber) {
    if (word.empty()) return;

    LinkedList<int>** listPtrPtr = idx.find(word);
    if (listPtrPtr == nullptr || *listPtrPtr == nullptr) {
        LinkedList<int>* newList = new LinkedList<int>();
        newList->push_back(lineNumber);
        idx.insert(word, newList, true);
    } else {
        LinkedList<int>* list = *listPtrPtr;
        if (list->isEmpty() || list->getAt(list->getSize() - 1) != lineNumber) {
            list->push_back(lineNumber);
        }
    }
}

void WordIndex::markDirty() {
    isDirty = true;
}

void WordIndex::rebuild(const Document& doc) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    clear();

    LinkedList<string> lines = doc.getAllLines();
    int currentLine = 1;

    for (const auto& lineText : lines) {
        string currentToken;
        for (char ch : lineText) {
            lastStepCount++;
            if (isalnum(static_cast<unsigned char>(ch))) {
                currentToken += static_cast<char>(tolower(static_cast<unsigned char>(ch)));
            } else {
                if (!currentToken.empty()) {
                    addWord(currentToken, currentLine);
                    currentToken.clear();
                }
            }
        }
        if (!currentToken.empty()) {
            addWord(currentToken, currentLine);
            currentToken.clear();
        }
        currentLine++;
    }

    isDirty = false;

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
}

const LinkedList<int>* WordIndex::linesFor(const string& word, const Document& doc) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    if (isDirty) {
        rebuild(doc);
    }

    string cleaned = cleanToken(word);
    LinkedList<int>** listPtrPtr = idx.find(cleaned);
    
    lastStepCount += idx.getLastStepCount();
    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();

    if (listPtrPtr != nullptr && *listPtrPtr != nullptr) {
        return *listPtrPtr;
    }
    return nullptr;
}

uint64_t WordIndex::getLastStepCount() const {
    return lastStepCount;
}

double WordIndex::getLastExecutionMicros() const {
    return lastExecutionMicros;
}
