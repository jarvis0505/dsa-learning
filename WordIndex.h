#ifndef WORDINDEX_H
#define WORDINDEX_H

#include <string>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <cctype>
#include "HashTable.h"
#include "LinkedList.h"
#include "Document.h"

/*
 * WordIndex Design & Trade-Off Commentary:
 * -----------------------------------------
 * Maintaining exact 1-based line numbers in a Hash Table during line insertions and deletions
 * requires updating line entries for every subsequent word in the document (O(W_doc) time).
 * 
 * To preserve O(1) editing speed for insertLine, deleteLine, replaceLine, undo, and redo:
 * Document edits mark WordIndex as 'isDirty'. On a FIND query, if isDirty is set, WordIndex
 * performs a full single-pass rebuild in O(W_doc) time and clears isDirty. Subsequent FIND
 * queries execute in O(1) average lookup + O(k) line output time.
 */

class WordIndex {
private:
    HashTable<std::string, LinkedList<int>*> idx;
    bool isDirty{true};

    uint64_t lastStepCount{0};
    double lastExecutionMicros{0.0};

    std::string cleanToken(const std::string& token) const;
    void addWord(const std::string& word, int lineNumber);

public:
    WordIndex() = default;

    // Rule of Three: Destructor
    ~WordIndex();

    // Rule of Three: Copy Constructor
    WordIndex(const WordIndex& other);

    // Rule of Three: Copy Assignment Operator
    WordIndex& operator=(const WordIndex& other);

    void markDirty();
    void rebuild(const Document& doc);

    const LinkedList<int>* linesFor(const std::string& word, const Document& doc);
    void clear();

    uint64_t getLastStepCount() const;
    double getLastExecutionMicros() const;
};

#endif // WORDINDEX_H
