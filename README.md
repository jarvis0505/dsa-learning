# Text and Code Editor Engine (C++17 Console Edition)

A high-performance C++17 console text and code editor engine built from scratch using custom data structures (`Stack<T>`, `LinkedList<T>`, and `HashTable<K,V>`).

Designed for data structure course performance benchmarks, this engine enforces strict $O(1)$ editing speed via a **two-stack cursor architecture**, provides single-pass bracket balance verification, fast hash-based bookmark management, word indexing, and tracks execution time (in microseconds) and step counts for every public operation.

---

## Key Features & Constraints

* **Zero STL Containers**: Uses **no** standard library containers (`std::vector`, `std::map`, `std::list`, `std::stack`, `std::sort`, etc.). All data management relies exclusively on custom template containers.
* **Performance Metrics**: Every public operation prints its execution duration in microseconds (`us`) and operation step count.
* **Rule of Three & RAII**: All container classes and domain models owning heap memory implement custom Destructors, Copy Constructors, and Copy Assignment Operators for zero memory leaks.
* **Two Execution Modes**:
  1. **Interactive Menu Mode**: Interactive console prompt (`Editor> `).
  2. **Script Mode**: Automated execution of command files passed as command-line arguments (`./editor script.txt`).

---

## Core Data Structures & Architecture

### 1. Custom Templates
* **`Stack<T>`** ([`Stack.h`](file:///c:/Users/VICTUS/Desktop/Final%20Project/Stack.h)): Generic node-based stack used for cursor position splitting and undo/redo history.
* **`LinkedList<T>`** ([`LinkedList.h`](file:///c:/Users/VICTUS/Desktop/Final%20Project/LinkedList.h)): Generic doubly-linked list with both `head` and `tail` pointers ($O(1)$ tail appends).
* **`HashTable<K,V>`** ([`HashTable.h`](file:///c:/Users/VICTUS/Desktop/Final%20Project/HashTable.h)): Separate-chaining hash table with FNV-1a string hashing and dynamic rehashing (doubles capacity at load factor $> 0.75$) to guarantee $O(1)$ average lookups for 1,000+ entries.

### 2. Domain Components
* **Document Engine** ([`Document.h`](file:///c:/Users/VICTUS/Desktop/Final%20Project/Document.h), [`Document.cpp`](file:///c:/Users/VICTUS/Desktop/Final%20Project/Document.cpp)): Uses two `Stack<string>` members (`aboveCursor` and `belowCursor`) to maintain the current line at top of `belowCursor`. Line edits (`INSERT`, `DELETE`, `REPLACE`) and undo/redo operate in $O(1)$ time at the cursor. `GOTO` takes $O(d)$ time where $d$ is line distance.
* **Bracket Checker** ([`BracketChecker.h`](file:///c:/Users/VICTUS/Desktop/Final%20Project/BracketChecker.h), [`BracketChecker.cpp`](file:///c:/Users/VICTUS/Desktop/Final%20Project/BracketChecker.cpp)): Performs a single-pass $O(N)$ syntax balance check over `()`, `{}`, `[]` using `Stack<BracketInfo>`.
* **Bookmark Manager** ([`BookmarkManager.h`](file:///c:/Users/VICTUS/Desktop/Final%20Project/BookmarkManager.h), [`BookmarkManager.cpp`](file:///c:/Users/VICTUS/Desktop/Final%20Project/BookmarkManager.cpp)): Stores name $\rightarrow$ line mappings in `HashTable<string, int>`. Rejects duplicate bookmark names with clear error messages.
* **Word Indexer** ([`WordIndex.h`](file:///c:/Users/VICTUS/Desktop/Final%20Project/WordIndex.h), [`WordIndex.cpp`](file:///c:/Users/VICTUS/Desktop/Final%20Project/WordIndex.cpp)): Case-insensitive token indexer using `HashTable<string, LinkedList<int>*>`. Uses a lazy rebuild strategy on edits to preserve $O(1)$ editing speed.

---

## File Structure

```text
├── Stack.h                  # Custom Stack<T> template
├── LinkedList.h             # Custom LinkedList<T> template with head & tail pointers
├── HashTable.h              # Custom HashTable<K,V> template with dynamic rehashing
├── EditRecord.h             # Plain struct for Undo/Redo edit history
├── BracketInfo.h            # Plain struct for line & col bracket tracking
├── Document.h / .cpp        # Two-stack text document editor engine
├── BracketChecker.h / .cpp  # Single-pass syntax bracket validator
├── BookmarkManager.h / .cpp # Bookmark manager using HashTable
├── WordIndex.h / .cpp       # Word search indexer
├── main.cpp                 # CLI driver (Interactive & Script modes)
├── build.bat                # Build script for MinGW GCC 13.1.0
├── data/
│   ├── manuscript.txt       # Sample 50-line manuscript file
│   └── large_manuscript.txt # Synthetic 5,000-line test manuscript
└── tests/
    ├── test_edge_cases.txt  # Edge case automated script suite
    └── test_large_file.txt   # Scale performance test script
```

---

## Build & Run Instructions

### Prerequisites
* GCC 13+ (or standard C++17 compiler).

### Compilation
Run the provided build script:
```cmd
build.bat
```
*(Or compile manually: `g++ -std=c++17 -Wall -Wextra -static Document.cpp BracketChecker.cpp BookmarkManager.cpp WordIndex.cpp main.cpp -o editor.exe`)*

### Launch Interactive Mode
```powershell
.\editor.exe
```
Inside interactive mode, enter commands at the `Editor> ` prompt.

### Launch Script Mode
Run automated test scripts:
```powershell
# Edge cases test suite
.\editor.exe tests/test_edge_cases.txt

# 5,000-line performance benchmark
.\editor.exe tests/test_large_file.txt
```

---

## Command Reference

| Command | Syntax | Description | Big-O Complexity |
| :--- | :--- | :--- | :--- |
| **`LOAD`** | `LOAD <filename>` | Loads document file line by line | $O(N)$ |
| **`SAVE`** | `SAVE <filename>` | Saves current document to file | $O(N)$ |
| **`INSERT`** | `INSERT <text>` | Inserts new line at cursor position | $O(1)$ |
| **`DELETE`** | `DELETE` | Deletes line at cursor position | $O(1)$ |
| **`REPLACE`** | `REPLACE <text>` | Replaces line content at cursor | $O(1)$ |
| **`UP`** | `UP` | Moves cursor up 1 line | $O(1)$ |
| **`DOWN`** | `DOWN` | Moves cursor down 1 line | $O(1)$ |
| **`GOTO`** | `GOTO <line>` | Jumps cursor to line number | $O(d)$ ($d = \|n - \text{cur}\|$) |
| **`UNDO`** | `UNDO` | Reverts last edit operation | $O(1)$ |
| **`REDO`** | `REDO` | Re-applies last undone edit | $O(1)$ |
| **`CHECKBRACKETS`** | `CHECKBRACKETS` | Validates document syntax balance | $O(N)$ |
| **`BOOKMARK ADD`** | `BOOKMARK ADD <name>` | Adds bookmark for current line | $O(1)$ average |
| **`BOOKMARK GOTO`** | `BOOKMARK GOTO <name>` | Jumps cursor to bookmarked line | $O(1)$ average |
| **`BOOKMARK DELETE`** | `BOOKMARK DELETE <name>` | Deletes existing bookmark | $O(1)$ average |
| **`FIND`** | `FIND <word>` | Searches word occurrences | $O(1)$ avg lookup / $O(W_{\text{doc}})$ rebuild |
| **`PRINT`** | `PRINT` | Displays current line + context lines | $O(\text{context})$ |
| **`EXIT`** | `EXIT` | Terminates editor engine | $O(1)$ |

---

## Performance Benchmark Metrics (5,000 Line Document)

| Operation | Dataset Size | Execution Time (us) | Steps Count |
| :--- | :--- | :--- | :--- |
| **`LOAD`** | 5,000 lines | 114,258 us (114 ms) | 10,000 steps |
| **`GOTO 2500`** | Distance = 2,499 | 1,691 us (1.6 ms) | 2,499 steps |
| **`BOOKMARK ADD`** | 5,000 line document | 12.40 us | 3 steps |
| **`BOOKMARK GOTO`** | Table size 5,000 | 0.90 us | 2 steps |
| **`INSERT`** | Line 5000 | 1.70 us | 2 steps |
| **`UNDO`** | Line 5000 | 4.00 us | 2 steps |

---

## License & Credits

Developed for Data Structures & Algorithms Coursework (C++17 Console Edition).
