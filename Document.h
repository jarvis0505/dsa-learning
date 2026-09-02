#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <iostream>
#include "Stack.h"
#include "LinkedList.h"
#include "EditRecord.h"

class Document {
private:
    Stack<std::string> aboveCursor;
    Stack<std::string> belowCursor;

    Stack<EditRecord> undoStack;
    Stack<EditRecord> redoStack;

    int currentLineNum{1}; // 1-based index

    mutable uint64_t lastStepCount{0};
    mutable double lastExecutionMicros{0.0};

    // Helper method to move cursor to line n without logging undo/redo
    void internalJumpToLine(int targetLine);

public:
    Document() = default;
    ~Document() = default;
    Document(const Document& other) = default;
    Document& operator=(const Document& other) = default;

    bool moveUp();
    bool moveDown();

    /*
     * jumpToLine(n) takes O(d) time where d = |currentLineNum - targetLine|.
     * Per assignment speed rules: Since line edits (insert/delete/replace) and sequential navigation
     * at the cursor run in strictly O(1) time using the two-stack representation, moving to a target
     * line requires shifting d elements between aboveCursor and belowCursor stacks one by one.
     * This relative O(d) cost is optimal for interactive text editing patterns.
     */
    bool jumpToLine(int lineNumber);

    void insertLine(const std::string& text);
    bool deleteLine();
    bool replaceLine(const std::string& text);

    bool undo();
    bool redo();

    bool load(const std::string& filename);
    bool save(const std::string& filename);

    int getCurrentLineNumber() const;
    int getTotalLines() const;
    std::string getCurrentLine() const;

    void printContext(int radius = 2) const;

    // Helper to get copy of all lines in 1..N order for indexers/checkers
    LinkedList<std::string> getAllLines() const;

    uint64_t getLastStepCount() const;
    double getLastExecutionMicros() const;
};

#endif // DOCUMENT_H
