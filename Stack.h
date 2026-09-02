#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <chrono>
#include <cstdint>
#include <stdexcept>

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val, Node* n = nullptr) : data(val), next(n) {}
    };

    Node* topNode{nullptr};
    int size{0};

    mutable uint64_t lastStepCount{0};
    mutable double lastExecutionMicros{0.0};

    void copyFrom(const Stack<T>& other) {
        if (other.topNode == nullptr) {
            topNode = nullptr;
            size = 0;
            return;
        }
        
        // Reverse copy trick or array copy to preserve top-to-bottom order
        Node* currOther = other.topNode;
        Node* dummyHead = new Node(currOther->data);
        Node* currNew = dummyHead;
        currOther = currOther->next;

        while (currOther != nullptr) {
            currNew->next = new Node(currOther->data);
            currNew = currNew->next;
            currOther = currOther->next;
        }
        topNode = dummyHead;
        size = other.size;
    }

public:
    Stack() = default;

    // Rule of Three: Destructor
    ~Stack() {
        clear();
    }

    // Rule of Three: Copy Constructor
    Stack(const Stack<T>& other) {
        copyFrom(other);
    }

    // Rule of Three: Copy Assignment Operator
    Stack<T>& operator=(const Stack<T>& other) {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }

    void push(const T& val) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        topNode = new Node(val, topNode);
        size++;
        lastStepCount++;

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
    }

    T pop() {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        if (isEmpty()) {
            auto end = std::chrono::high_resolution_clock::now();
            lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
            throw std::runtime_error("Stack underflow: pop from empty stack");
        }

        Node* temp = topNode;
        T poppedVal = temp->data;
        topNode = topNode->next;
        delete temp;
        size--;
        lastStepCount++;

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return poppedVal;
    }

    T& top() {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 1;
        if (isEmpty()) {
            auto end = std::chrono::high_resolution_clock::now();
            lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
            throw std::runtime_error("Stack empty: top on empty stack");
        }
        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return topNode->data;
    }

    const T& top() const {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 1;
        if (isEmpty()) {
            auto end = std::chrono::high_resolution_clock::now();
            lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
            throw std::runtime_error("Stack empty: top on empty stack");
        }
        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    int getSize() const {
        return size;
    }

    void clear() {
        while (topNode != nullptr) {
            Node* temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
        size = 0;
    }

    uint64_t getLastStepCount() const {
        return lastStepCount;
    }

    double getLastExecutionMicros() const {
        return lastExecutionMicros;
    }
};

#endif // STACK_H
