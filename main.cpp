#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>

#include "Document.h"
#include "BracketChecker.h"
#include "BookmarkManager.h"
#include "WordIndex.h"

using namespace std;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

string toUpper(const string& str) {
    string res = str;
    for (char& c : res) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    return res;
}

void printMetrics(double micros, uint64_t steps) {
    cout << " [Time: " << fixed << setprecision(2) << micros << " us | Steps: " << steps << "]\n";
}

bool executeCommand(const string& rawLine, Document& doc, BracketChecker& checker, BookmarkManager& bookmarks, WordIndex& wordIdx) {
    string line = trim(rawLine);
    if (line.empty() || line[0] == '#') {
        return true; // Ignore empty lines and comments in scripts
    }

    stringstream ss(line);
    string cmd;
    ss >> cmd;
    string upperCmd = toUpper(cmd);

    if (upperCmd == "EXIT" || upperCmd == "QUIT") {
        cout << "[EXIT] Terminating editor engine.\n";
        return false;
    }
    else if (upperCmd == "LOAD") {
        string filename;
        ss >> filename;
        if (filename.empty()) {
            cout << "[LOAD Error] Usage: LOAD <filename>\n";
        } else {
            if (doc.load(filename)) {
                bookmarks.clear();
                wordIdx.markDirty();
                cout << "[LOAD] Loaded '" << filename << "' successfully. Total lines: " << doc.getTotalLines();
                printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
            } else {
                cout << "[LOAD Error] Failed to open file '" << filename << "'.";
                printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
            }
        }
    }
    else if (upperCmd == "SAVE") {
        string filename;
        ss >> filename;
        if (filename.empty()) {
            cout << "[SAVE Error] Usage: SAVE <filename>\n";
        } else {
            if (doc.save(filename)) {
                cout << "[SAVE] Saved document to '" << filename << "'.";
                printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
            } else {
                cout << "[SAVE Error] Failed to save file '" << filename << "'.";
                printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
            }
        }
    }
    else if (upperCmd == "INSERT") {
        string rest;
        getline(ss, rest);
        string text = trim(rest);
        doc.insertLine(text);
        wordIdx.markDirty();
        cout << "[INSERT] Inserted line at position " << doc.getCurrentLineNumber() << ".";
        printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
    }
    else if (upperCmd == "DELETE") {
        if (doc.deleteLine()) {
            wordIdx.markDirty();
            cout << "[DELETE] Deleted line at position " << doc.getCurrentLineNumber() << ".";
            printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
        } else {
            cout << "[DELETE Error] Cannot delete from empty document or past EOF.";
            printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
        }
    }
    else if (upperCmd == "REPLACE") {
        string rest;
        getline(ss, rest);
        string text = trim(rest);
        if (doc.replaceLine(text)) {
            wordIdx.markDirty();
            cout << "[REPLACE] Replaced line at position " << doc.getCurrentLineNumber() << ".";
            printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
        } else {
            cout << "[REPLACE Error] Cannot replace on empty document.";
            printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
        }
    }
    else if (upperCmd == "UP") {
        if (doc.moveUp()) {
            cout << "[UP] Cursor moved to line " << doc.getCurrentLineNumber() << ".";
        } else {
            cout << "[UP] Cursor already at line 1.";
        }
        printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
    }
    else if (upperCmd == "DOWN") {
        if (doc.moveDown()) {
            cout << "[DOWN] Cursor moved to line " << doc.getCurrentLineNumber() << ".";
        } else {
            cout << "[DOWN] Cursor already at end of file (line " << doc.getCurrentLineNumber() << ").";
        }
        printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
    }
    else if (upperCmd == "GOTO") {
        int lineNum = 0;
        if (ss >> lineNum) {
            if (doc.jumpToLine(lineNum)) {
                cout << "[GOTO] Jumped to line " << doc.getCurrentLineNumber() << ".";
            } else {
                cout << "[GOTO Error] Invalid line number " << lineNum << ". Clamped / stays at line " << doc.getCurrentLineNumber() << ".";
            }
        } else {
            cout << "[GOTO Error] Usage: GOTO <line_number>";
        }
        printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
    }
    else if (upperCmd == "UNDO") {
        if (doc.undo()) {
            wordIdx.markDirty();
            cout << "[UNDO] Undo successful. Current line: " << doc.getCurrentLineNumber() << ".";
        } else {
            cout << "[UNDO] Nothing to undo.";
        }
        printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
    }
    else if (upperCmd == "REDO") {
        if (doc.redo()) {
            wordIdx.markDirty();
            cout << "[REDO] Redo successful. Current line: " << doc.getCurrentLineNumber() << ".";
        } else {
            cout << "[REDO] Nothing to redo.";
        }
        printMetrics(doc.getLastExecutionMicros(), doc.getLastStepCount());
    }
    else if (upperCmd == "CHECKBRACKETS") {
        auto result = checker.checkBrackets(doc);
        if (result.isValid) {
            cout << "[CHECKBRACKETS] OK: " << result.errorMessage;
        } else {
            cout << "[CHECKBRACKETS] Syntax Error: " << result.errorMessage;
        }
        printMetrics(checker.getLastExecutionMicros(), checker.getLastStepCount());
    }
    else if (upperCmd == "BOOKMARK") {
        string subCmd;
        ss >> subCmd;
        string upperSub = toUpper(subCmd);

        if (upperSub == "ADD") {
            string name;
            ss >> name;
            if (name.empty()) {
                cout << "[BOOKMARK ADD Error] Usage: BOOKMARK ADD <name>\n";
            } else {
                int curLine = doc.getCurrentLineNumber();
                if (bookmarks.addBookmark(name, curLine)) {
                    cout << "[BOOKMARK ADD] Added bookmark '" << name << "' -> line " << curLine << ".";
                } else {
                    cout << "[BOOKMARK ADD Error] Bookmark '" << name << "' already exists (duplicate rejected).";
                }
                printMetrics(bookmarks.getLastExecutionMicros(), bookmarks.getLastStepCount());
            }
        }
        else if (upperSub == "GOTO") {
            string name;
            ss >> name;
            if (name.empty()) {
                cout << "[BOOKMARK GOTO Error] Usage: BOOKMARK GOTO <name>\n";
            } else {
                int targetLine = bookmarks.jumpTo(name);
                if (targetLine != -1) {
                    doc.jumpToLine(targetLine);
                    cout << "[BOOKMARK GOTO] Jumped to line " << doc.getCurrentLineNumber() << " for bookmark '" << name << "'.";
                } else {
                    cout << "[BOOKMARK GOTO Error] Bookmark '" << name << "' not found.";
                }
                printMetrics(bookmarks.getLastExecutionMicros(), bookmarks.getLastStepCount());
            }
        }
        else if (upperSub == "DELETE") {
            string name;
            ss >> name;
            if (name.empty()) {
                cout << "[BOOKMARK DELETE Error] Usage: BOOKMARK DELETE <name>\n";
            } else {
                if (bookmarks.removeBookmark(name)) {
                    cout << "[BOOKMARK DELETE] Deleted bookmark '" << name << "'.";
                } else {
                    cout << "[BOOKMARK DELETE Error] Bookmark '" << name << "' not found.";
                }
                printMetrics(bookmarks.getLastExecutionMicros(), bookmarks.getLastStepCount());
            }
        }
        else {
            cout << "[BOOKMARK Error] Unknown bookmark subcommand '" << subCmd << "'.\n";
        }
    }
    else if (upperCmd == "FIND") {
        string word;
        ss >> word;
        if (word.empty()) {
            cout << "[FIND Error] Usage: FIND <word>\n";
        } else {
            const LinkedList<int>* lines = wordIdx.linesFor(word, doc);
            if (lines != nullptr && !lines->isEmpty()) {
                cout << "[FIND] Word '" << word << "' found on line(s): ";
                bool first = true;
                for (const auto& lNum : *lines) {
                    if (!first) cout << ", ";
                    cout << lNum;
                    first = false;
                }
                printMetrics(wordIdx.getLastExecutionMicros(), wordIdx.getLastStepCount());
            } else {
                cout << "[FIND] Word '" << word << "' not found in document.";
                printMetrics(wordIdx.getLastExecutionMicros(), wordIdx.getLastStepCount());
            }
        }
    }
    else if (upperCmd == "PRINT") {
        doc.printContext(2);
    }
    else {
        cout << "[ERROR] Unknown command: '" << cmd << "'\n";
    }

    return true;
}

int main(int argc, char* argv[]) {
    Document doc;
    BracketChecker checker;
    BookmarkManager bookmarks;
    WordIndex wordIdx;

    cout << "========================================================\n";
    cout << "   Text and Code Editor Engine (C++17 Console Edition)\n";
    cout << "========================================================\n";

    if (argc > 1) {
        string scriptPath = argv[1];
        cout << "[MODE] Executing script mode: '" << scriptPath << "'\n\n";

        ifstream scriptFile(scriptPath);
        if (!scriptFile.is_open()) {
            cerr << "[FATAL] Failed to open script file: " << scriptPath << "\n";
            return 1;
        }

        string commandLine;
        while (getline(scriptFile, commandLine)) {
            cout << "Command> " << commandLine << "\n";
            if (!executeCommand(commandLine, doc, checker, bookmarks, wordIdx)) {
                break;
            }
        }
        scriptFile.close();
        cout << "\n[SCRIPT COMPLETED]\n";
    } else {
        cout << "[MODE] Interactive Menu Mode. Type 'EXIT' to quit.\n";
        cout << "Commands: LOAD, SAVE, INSERT, DELETE, REPLACE, UP, DOWN, GOTO, UNDO, REDO,\n";
        cout << "          CHECKBRACKETS, BOOKMARK ADD/GOTO/DELETE, FIND, PRINT, EXIT\n\n";

        string inputLine;
        while (true) {
            cout << "Editor> ";
            if (!getline(cin, inputLine)) {
                break;
            }
            if (!executeCommand(inputLine, doc, checker, bookmarks, wordIdx)) {
                break;
            }
        }
    }

    return 0;
}
