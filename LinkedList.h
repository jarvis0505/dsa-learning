#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include <chrono>
#include <cstdint>
#include <stdexcept>

template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node* prev;
        Node(const T& val, Node* p = nullptr, Node* n = nullptr)
            : data(val), next(n), prev(p) {}
    };

    Node* head{nullptr};
    Node* tail{nullptr};
    int size{0};

    mutable uint64_t lastStepCount{0};
    mutable double lastExecutionMicros{0.0};

    void copyFrom(const LinkedList<T>& other) {
        head = nullptr;
        tail = nullptr;
        size = 0;
        Node* curr = other.head;
        while (curr != nullptr) {
            push_back(curr->data);
            curr = curr->next;
        }
    }

public:
    LinkedList() = default;

    // Rule of Three: Destructor
    ~LinkedList() {
        clear();
    }

    // Rule of Three: Copy Constructor
    LinkedList(const LinkedList<T>& other) {
        copyFrom(other);
    }

    // Rule of Three: Copy Assignment Operator
    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }

    void push_back(const T& val) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        Node* newNode = new Node(val, tail, nullptr);
        lastStepCount++;

        if (tail != nullptr) {
            tail->next = newNode;
            lastStepCount++;
        } else {
            head = newNode;
            lastStepCount++;
        }
        tail = newNode;
        size++;

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
    }

    void push_front(const T& val) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        Node* newNode = new Node(val, nullptr, head);
        lastStepCount++;

        if (head != nullptr) {
            head->prev = newNode;
            lastStepCount++;
        } else {
            tail = newNode;
            lastStepCount++;
        }
        head = newNode;
        size++;

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
    }

    T pop_front() {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        if (isEmpty()) {
            auto end = std::chrono::high_resolution_clock::now();
            lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
            throw std::runtime_error("LinkedList underflow: pop_front from empty list");
        }

        Node* temp = head;
        T val = temp->data;
        head = head->next;
        lastStepCount++;

        if (head != nullptr) {
            head->prev = nullptr;
            lastStepCount++;
        } else {
            tail = nullptr;
            lastStepCount++;
        }

        delete temp;
        size--;

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return val;
    }

    T& getAt(int index) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        if (index < 0 || index >= size) {
            auto end = std::chrono::high_resolution_clock::now();
            lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
            throw std::out_of_range("LinkedList index out of bounds");
        }

        Node* curr = head;
        for (int i = 0; i < index; ++i) {
            curr = curr->next;
            lastStepCount++;
        }

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return curr->data;
    }

    const T& getAt(int index) const {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        if (index < 0 || index >= size) {
            auto end = std::chrono::high_resolution_clock::now();
            lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
            throw std::out_of_range("LinkedList index out of bounds");
        }

        Node* curr = head;
        for (int i = 0; i < index; ++i) {
            curr = curr->next;
            lastStepCount++;
        }

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return curr->data;
    }

    bool removeAt(int index) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        if (index < 0 || index >= size) {
            auto end = std::chrono::high_resolution_clock::now();
            lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
            return false;
        }

        Node* curr = head;
        for (int i = 0; i < index; ++i) {
            curr = curr->next;
            lastStepCount++;
        }

        if (curr->prev != nullptr) {
            curr->prev->next = curr->next;
            lastStepCount++;
        } else {
            head = curr->next;
            lastStepCount++;
        }

        if (curr->next != nullptr) {
            curr->next->prev = curr->prev;
            lastStepCount++;
        } else {
            tail = curr->prev;
            lastStepCount++;
        }

        delete curr;
        size--;

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return true;
    }

    bool removeValue(const T& val) {
        auto start = std::chrono::high_resolution_clock::now();
        lastStepCount = 0;

        Node* curr = head;
        while (curr != nullptr) {
            lastStepCount++;
            if (curr->data == val) {
                if (curr->prev != nullptr) {
                    curr->prev->next = curr->next;
                } else {
                    head = curr->next;
                }
                if (curr->next != nullptr) {
                    curr->next->prev = curr->prev;
                } else {
                    tail = curr->prev;
                }
                delete curr;
                size--;
                auto end = std::chrono::high_resolution_clock::now();
                lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
                return true;
            }
            curr = curr->next;
        }

        auto end = std::chrono::high_resolution_clock::now();
        lastExecutionMicros = std::chrono::duration<double, std::micro>(end - start).count();
        return false;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    int getSize() const {
        return size;
    }

    void clear() {
        Node* curr = head;
        while (curr != nullptr) {
            Node* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    uint64_t getLastStepCount() const {
        return lastStepCount;
    }

    double getLastExecutionMicros() const {
        return lastExecutionMicros;
    }

    // Helper iterator for traversal
    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* ptr) : current(ptr) {}
        T& operator*() { return current->data; }
        Iterator& operator++() { if (current) current = current->next; return *this; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    class ConstIterator {
    private:
        const Node* current;
    public:
        ConstIterator(const Node* ptr) : current(ptr) {}
        const T& operator*() const { return current->data; }
        ConstIterator& operator++() { if (current) current = current->next; return *this; }
        bool operator!=(const ConstIterator& other) const { return current != other.current; }
    };

    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }
};

#endif // LINKEDLIST_H
