#ifndef BRACKETCHECKER_H
#define BRACKETCHECKER_H

#include <iostream>
#include <chrono>
#include <cstdint>
#include "Document.h"
#include "Stack.h"
#include "BracketInfo.h"

class BracketChecker {
private:
    uint64_t lastStepCount{0};
    double lastExecutionMicros{0.0};

public:
    BracketChecker() = default;
    ~BracketChecker() = default;

    struct Result {
        bool isValid{true};
        std::string errorMessage;
        int errorLine{0};
        int errorCol{0};
    };

    Result checkBrackets(const Document& doc);

    uint64_t getLastStepCount() const;
    double getLastExecutionMicros() const;
};

#endif // BRACKETCHECKER_H
