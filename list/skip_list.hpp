#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <memory>
#include <iterator>

template<typename T>
class SkipList {
private:
    static constexpr float PROBABILITY = 0.5;
    static constexpr int MAX_LEVEL = 16;

    struct Node {
        T data;
        std::vector<std::unique_ptr<Node>> forward;
        std::vector<Node*> raw_forward;

        Node(const T& val, int level)
            : data(val), forward(level + 1), raw_forward(level + 1, nullptr) {}
    };

    std::unique_ptr<Node> head;
    size_t size_;
    int level;
    std::mt19937 rng;
    std::uniform_real_distribution<> dist;

    int randomLevel() {
        int lvl = 0;
        while (dist(rng) < PROBABILITY && lvl < MAX_LEVEL)
            ++lvl;
        return lvl;
    }

public:
    SkipList()
        : head(std::make_unique<Node>(T{}, MAX_LEVEL)),
          size_(0),
          level(0),
          rng(std::random_device{}()),
          dist(0.0, 1.0) {}

    void insert(const T& value) {
    std::vector<Node*> update(MAX_LEVEL + 1, nullptr);
    Node* curr = head.get();

    for (int i = level; i >= 0; --i) {
        while (curr->raw_forward[i] && curr->raw_forward[i]->data < value)
            curr = curr->raw_forward[i];
        update[i] = curr;
    }

    Node* next = curr->raw_forward[0];
    if (next && next->data == value)
        return;

    int newLevel = randomLevel();
    if (newLevel > level) {
        for (int i = level + 1; i <= newLevel; ++i)
            update[i] = head.get();
        level = newLevel;
    }

    auto newNode = std::make_unique<Node>(value, newLevel);
    Node* newNodeRaw = newNode.get();

    for (int i = 0; i <= newLevel; ++i) {
        newNode->forward[i] = std::move(update[i]->forward[i]);
        newNode->raw_forward[i] = newNode->forward[i].get();
    }

    for (int i = 0; i <= newLevel; ++i) {
        update[i]->forward[i] = std::move(newNode);
        update[i]->raw_forward[i] = newNodeRaw;
    }

    ++size_;
}

    bool erase(const T& value) {
        std::vector<Node*> update(MAX_LEVEL + 1, nullptr);
        Node* curr = head.get();

        for (int i = level; i >= 0; --i) {
            while (curr->raw_forward[i] && curr->raw_forward[i]->data < value)
                curr = curr->raw_forward[i];
            update[i] = curr;
        }

        Node* target = curr->raw_forward[0];
        if (!target || target->data != value)
            return false;

        for (int i = 0; i <= level; ++i) {
            if (update[i]->raw_forward[i] != target)
                break;
            update[i]->forward[i] = std::move(target->forward[i]);
            update[i]->raw_forward[i] = update[i]->forward[i].get();
        }

        while (level > 0 && !head->raw_forward[level])
            --level;

        --size_;
        return true;
    }

    bool find(const T& value) const {
        Node* curr = head.get();
        for (int i = level; i >= 0; --i) {
            while (curr->raw_forward[i] && curr->raw_forward[i]->data < value)
                curr = curr->raw_forward[i];
        }
        curr = curr->raw_forward[0];
        return curr && curr->data == value;
    }

    T& at(size_t index) {
        if (index >= size_)
            throw std::out_of_range("Index out of range");
        Node* curr = head->raw_forward[0];
        for (size_t i = 0; i < index; ++i)
            curr = curr->raw_forward[0];
        return curr->data;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    class iterator {
        Node* ptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using reference = T&;
        using pointer = T*;

        explicit iterator(Node* p) : ptr(p) {}

        T& operator*() const { return ptr->data; }
        T* operator->() const { return &(ptr->data); }

        iterator& operator++() {if (ptr) ptr = ptr->raw_forward[0];
            return *this;
        }

        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
    };

    iterator begin() { return iterator(head->raw_forward[0]); }
    iterator end() { return iterator(nullptr); }
};

#endif // SKIP_LIST_HPP