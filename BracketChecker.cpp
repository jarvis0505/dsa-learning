#include "BracketChecker.h"

using namespace std;

BracketChecker::Result BracketChecker::checkBrackets(const Document& doc) {
    auto start = chrono::high_resolution_clock::now();
    lastStepCount = 0;

    Result res;
    Stack<BracketInfo> openStack;
    LinkedList<string> lines = doc.getAllLines();

    int currentLine = 1;
    for (const auto& lineText : lines) {
        for (size_t col = 0; col < lineText.length(); ++col) {
            char ch = lineText[col];
            lastStepCount++;

            if (ch == '(' || ch == '{' || ch == '[') {
                openStack.push(BracketInfo(ch, currentLine, static_cast<int>(col + 1)));
            } else if (ch == ')' || ch == '}' || ch == ']') {
                if (openStack.isEmpty()) {
                    res.isValid = false;
                    res.errorLine = currentLine;
                    res.errorCol = static_cast<int>(col + 1);
                    res.errorMessage = string("Unmatched closing bracket '") + ch + "' at line " +
                                       to_string(res.errorLine) + ", col " + to_string(res.errorCol);
                    auto end = chrono::high_resolution_clock::now();
                    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
                    return res;
                }

                BracketInfo openInfo = openStack.pop();
                char expectedOpen = '\0';
                if (ch == ')') expectedOpen = '(';
                else if (ch == '}') expectedOpen = '{';
                else if (ch == ']') expectedOpen = '[';

                if (openInfo.symbol != expectedOpen) {
                    res.isValid = false;
                    res.errorLine = currentLine;
                    res.errorCol = static_cast<int>(col + 1);
                    res.errorMessage = string("Mismatched closing bracket '") + ch + "' at line " +
                                       to_string(res.errorLine) + ", col " + to_string(res.errorCol) +
                                       " (opened '" + openInfo.symbol + "' at line " +
                                       to_string(openInfo.line) + ", col " + to_string(openInfo.col) + ")";
                    auto end = chrono::high_resolution_clock::now();
                    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
                    return res;
                }
            }
        }
        currentLine++;
    }

    if (!openStack.isEmpty()) {
        BracketInfo unclosed = openStack.pop();
        res.isValid = false;
        res.errorLine = unclosed.line;
        res.errorCol = unclosed.col;
        res.errorMessage = string("Unclosed bracket '") + unclosed.symbol + "' opened at line " +
                           to_string(unclosed.line) + ", col " + to_string(unclosed.col);
    } else {
        res.isValid = true;
        res.errorMessage = "All brackets are balanced.";
    }

    auto end = chrono::high_resolution_clock::now();
    lastExecutionMicros = chrono::duration<double, micro>(end - start).count();
    return res;
}

uint64_t BracketChecker::getLastStepCount() const {
    return lastStepCount;
}

double BracketChecker::getLastExecutionMicros() const {
    return lastExecutionMicros;
}
