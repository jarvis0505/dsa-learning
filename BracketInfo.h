#ifndef BRACKETINFO_H
#define BRACKETINFO_H

struct BracketInfo {
    char symbol;
    int line; // 1-based index
    int col;  // 1-based index

    BracketInfo() : symbol('\0'), line(0), col(0) {}
    BracketInfo(char sym, int l, int c) : symbol(sym), line(l), col(c) {}
};

#endif // BRACKETINFO_H
