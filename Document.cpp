#include "Document.h"
#include <fstream>
#include <chrono>

using namespace std;

int Document::getCurrentLineNumber() const {
    if (belowCursor.isEmpty() && aboveCursor.isEmpty()) {
        return 0;
    }
    return aboveCursor.getSize() + 1;
}

int Document::getTotalLines() const {
    return aboveCursor.getSize() + belowCursor.getSize();
}

string Document::getCurrentLine() const {
    if (belowCursor.isEmpty()) {
        return "";
    }
    return belowCursor.top();
}

void Document::internalJumpToLine(int targetLine) {
    int total = getTotalLines();
    if (total == 0) return;
    if (targetLine < 1) targetLine = 1;
    if (targetLine > total) targetLine = total;

    while (getCurrentLineNumber() < targetLine && belowCursor.getSize() > 1) {
        aboveCursor.push(belowCursor.pop());
    }
    while (getCurrentLineNumber() > targetLine && !aboveCursor.isEmpty()) {
        belowCursor.push(aboveCursor.pop());
    }
}

bool Document::moveUp() {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    if (aboveCursor.isEmpty()) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    belowCursor.push(aboveCursor.pop());
    lastStepCount++;

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

bool Document::moveDown() {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    if (belowCursor.getSize() <= 1) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    aboveCursor.push(belowCursor.pop());
    lastStepCount++;

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

bool Document::jumpToLine(int lineNumber) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    int total = getTotalLines();
    if (total == 0 || lineNumber < 1 || lineNumber > total) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    while (getCurrentLineNumber() < lineNumber && belowCursor.getSize() > 1) {
        aboveCursor.push(belowCursor.pop());
        lastStepCount++;
    }
    while (getCurrentLineNumber() > lineNumber && !aboveCursor.isEmpty()) {
        belowCursor.push(aboveCursor.pop());
        lastStepCount++;
    }

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

void Document::insertLine(const string& text) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    int curLine = getCurrentLineNumber();
    if (curLine == 0) curLine = 1;

    belowCursor.push(text);
    lastStepCount++;

    undoStack.push(EditRecord(EditType::INSERT, curLine, "", text));
    redoStack.clear();
    lastStepCount++;

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
}

bool Document::deleteLine() {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    if (belowCursor.isEmpty()) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    int curLine = getCurrentLineNumber();
    string oldText = belowCursor.pop();
    lastStepCount++;

    if (belowCursor.isEmpty() && !aboveCursor.isEmpty()) {
        belowCursor.push(aboveCursor.pop());
        lastStepCount++;
    }

    undoStack.push(EditRecord(EditType::DELETE, curLine, oldText, ""));
    redoStack.clear();
    lastStepCount++;

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

bool Document::replaceLine(const string& text) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    if (belowCursor.isEmpty()) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    int curLine = getCurrentLineNumber();
    string oldText = belowCursor.pop();
    belowCursor.push(text);
    lastStepCount += 2;

    undoStack.push(EditRecord(EditType::REPLACE, curLine, oldText, text));
    redoStack.clear();
    lastStepCount++;

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

bool Document::undo() {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    if (undoStack.isEmpty()) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    EditRecord rec = undoStack.pop();
    lastStepCount++;

    internalJumpToLine(rec.lineNumber);

    if (rec.type == EditType::INSERT) {
        if (!belowCursor.isEmpty()) {
            belowCursor.pop();
            lastStepCount++;
        }
    } else if (rec.type == EditType::DELETE) {
        belowCursor.push(rec.oldContent);
        lastStepCount++;
    } else if (rec.type == EditType::REPLACE) {
        if (!belowCursor.isEmpty()) {
            belowCursor.pop();
        }
        belowCursor.push(rec.oldContent);
        lastStepCount += 2;
    }

    redoStack.push(rec);

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

bool Document::redo() {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    if (redoStack.isEmpty()) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    EditRecord rec = redoStack.pop();
    lastStepCount++;

    internalJumpToLine(rec.lineNumber);

    if (rec.type == EditType::INSERT) {
        belowCursor.push(rec.newContent);
        lastStepCount++;
    } else if (rec.type == EditType::DELETE) {
        if (!belowCursor.isEmpty()) {
            belowCursor.pop();
            lastStepCount++;
        }
    } else if (rec.type == EditType::REPLACE) {
        if (!belowCursor.isEmpty()) {
            belowCursor.pop();
        }
        belowCursor.push(rec.newContent);
        lastStepCount += 2;
    }

    undoStack.push(rec);

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

bool Document::load(const string& filename) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    ifstream inFile(filename);
    if (!inFile.is_open()) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    aboveCursor.clear();
    belowCursor.clear();
    undoStack.clear();
    redoStack.clear();

    LinkedList<string> lines;
    string line;
    while (getline(inFile, line)) {
        lines.push_back(line);
        lastStepCount++;
    }
    inFile.close();

    for (int i = lines.getSize() - 1; i >= 0; --i) {
        belowCursor.push(lines.getAt(i));
        lastStepCount++;
    }

    currentLineNum = 1;

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

bool Document::save(const string& filename) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    ofstream outFile(filename);
    if (!outFile.is_open()) {
        auto end = chrono::high_resolution_clock::now();
        lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
        return false;
    }

    LinkedList<string> lines = getAllLines();
    for (const auto& line : lines) {
        outFile << line << "\n";
        lastStepCount++;
    }

    outFile.close();

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return true;
}

void Document::printContext(int radius) const {
    LinkedList<string> lines = getAllLines();
    int total = lines.getSize();
    int current = getCurrentLineNumber();

    int startLine = (current - radius < 1) ? 1 : current - radius;
    int endLine = (current + radius > total) ? total : current + radius;

    cout << "--- Document View (Line " << current << " / " << total << ") ---\n";
    if (total == 0) {
        cout << "[Empty Document]\n";
        cout << "--------------------------------------------\n";
        return;
    }

    for (int i = startLine; i <= endLine; ++i) {
        if (i == current) {
            cout << " > " << i << ": " << lines.getAt(i - 1) << "\n";
        } else {
            cout << "   " << i << ": " << lines.getAt(i - 1) << "\n";
        }
    }
    cout << "--------------------------------------------\n";
}

LinkedList<string> Document::getAllLines() const {
    LinkedList<string> result;

    Stack<string> tempAbove = aboveCursor;
    Stack<string> reversedAbove;
    while (!tempAbove.isEmpty()) {
        reversedAbove.push(tempAbove.pop());
    }
    while (!reversedAbove.isEmpty()) {
        result.push_back(reversedAbove.pop());
    }

    Stack<string> tempBelow = belowCursor;
    while (!tempBelow.isEmpty()) {
        result.push_back(tempBelow.pop());
    }

    return result;
}

uint64_t Document::getLastStepCount() const {
    return lastStepCount;
}

double Document::getLastExecutionMicros() const {
    return lastExecutionMicros;
}
