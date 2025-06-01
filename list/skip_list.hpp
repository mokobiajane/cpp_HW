#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <memory>
#include <vector>
#include <random>
#include <stdexcept>

template<typename T>
class SkipList {
private:
    struct Node {
        T data;
        std::vector<std::unique_ptr<Node>> forward;

        Node(const T& value, int level) : data(value), forward(level + 1) {}
    };

    std::unique_ptr<Node> head;
    int level;
    static constexpr int max_level = 16;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    int randomLevel() {
        int lvl = 0;
        while (dis(gen) < 0.5 && lvl < max_level)
            lvl++;
        return lvl;
    }

public:
    SkipList() : level(0), gen(std::random_device{}()), dis(0.0, 1.0) {
        head = std::make_unique<Node>(T{}, max_level);
    }

    bool find(const T& value) const {
        Node* curr = head.get();
        for (int i = level; i >= 0; i--) {
            while (curr->forward[i] && curr->forward[i]->data < value)
                curr = curr->forward[i].get();
        }
        curr = curr->forward[0].get();
        return curr && curr->data == value;
    }

    void insert(const T& value) {
    std::vector<Node*> update(max_level + 1, nullptr);
    Node* curr = head.get();

    for (int i = level; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->data < value)
            curr = curr->forward[i].get();
        update[i] = curr;
    }

    curr = curr->forward[0].get();

    if (curr && curr->data == value) {
        return; // value already exists, no duplicates
    }

    int newLevel = randomLevel();
    if (newLevel > level) {
        for (int i = level + 1; i <= newLevel; i++) {
            update[i] = head.get();
        }
        level = newLevel;
    }

    // Create the new node
    auto newNode = std::make_unique<Node>(value, newLevel);

    // Set the new node's forward pointers to current next nodes
    for (int i = 0; i <= newLevel; i++) {
        newNode->forward[i] = std::move(update[i]->forward[i]);
    }

    // Link the update nodes to the new node (transfer ownership)
    for (int i = 0; i <= newLevel; i++) {
        update[i]->forward[i] = std::move(newNode);
    }
}
    bool erase(const T& value) {
        std::vector<Node*> update(max_level + 1, nullptr);
        Node* curr = head.get();

        for (int i = level; i >= 0; i--) {
            while (curr->forward[i] && curr->forward[i]->data < value)
                curr = curr->forward[i].get();
            update[i] = curr;
        }

        curr = curr->forward[0].get();

        if (!curr || curr->data != value)
            return false; // not found

        for (int i = 0; i <= level; i++) {
            if (update[i]->forward[i].get() != curr)
                break;
            update[i]->forward[i] = std::move(curr->forward[i]);
        }

        while (level > 0 && !head->forward[level])
            level--;

        return true;
    }

    T& at(size_t index) {
        Node* curr = head->forward[0].get();
        size_t i = 0;
        while (curr && i < index) {
            curr = curr->forward[0].get();
            i++;
        }
        if (!curr)
            throw std::out_of_range("Index out of range");
        return curr->data;
    }

    // Iterator implementation for range-based for loops
    class iterator {
        Node* ptr;
    public:
        explicit iterator(Node* node = nullptr) : ptr(node) {}

        T& operator*() const { return ptr->data; }

        iterator& operator++() {
            if (ptr) ptr = ptr->forward[0].get();
            return *this;
        }

        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
        bool operator==(const iterator& other) const { return ptr == other.ptr; }
    };

    iterator begin() { return iterator(head->forward[0].get()); }
    iterator end() { return iterator(nullptr); }
};

#endif // SKIP_LIST_HPP