#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <memory>
#include <vector>
#include <random>
#include <stdexcept>
#include <concepts>
#include <utility> // для std::swap
#include <cstddef> // для size_t

template<typename T>
concept SkipListValue = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
};

template<SkipListValue T>
class SkipList {
private:
    struct Node {
        T data;
        std::vector<std::unique_ptr<Node>> forward;

        Node(const T& value, int level) : data(value), forward(level + 1) {}
    };

    std::unique_ptr<Node> head;
    int level;
    size_t node_count = 0;
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

    // Размер списка
    size_t size() const noexcept {
        return node_count;
    }

    bool empty() const noexcept {
        return size() == 0;
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

    for (int i = level; i >= 0; --i) {
        while (curr->forward[i] && curr->forward[i]->data < value) {
            curr = curr->forward[i].get();
        }
        update[i] = curr;
    }

    curr = curr->forward[0].get();

    // Avoid duplicates
    if (curr && curr->data == value) return;

    int newLevel = randomLevel();

    if (newLevel > level) {
        for (int i = level + 1; i <= newLevel; ++i) {
            update[i] = head.get();
        }
        level = newLevel;
    }

    
    auto newNode = std::make_unique<Node>(value, newLevel);
    Node* newNodeRaw = newNode.get();  

    for (int i = 0; i <= newLevel; ++i) {
        newNode->forward[i] = std::move(update[i]->forward[i]);
        update[i]->forward[i] = nullptr; 
    }

    update[0]->forward[0] = std::move(newNode);

    for (int i = 1; i <= newLevel; ++i) {
        update[i]->forward[i].reset(newNodeRaw); 
    }

    node_count++;
}

    bool erase(const T& value) {
    std::vector<Node*> update(max_level + 1, nullptr);
    Node* curr = head.get();

    for (int i = level; i >= 0; --i) {
        while (curr->forward[i] && curr->forward[i]->data < value)
            curr = curr->forward[i].get();
        update[i] = curr;
    }

    curr = curr->forward[0].get();

    if (!curr || curr->data != value)
        return false;

    for (int i = 0; i <= level; ++i) {
        if (!update[i]->forward[i] || update[i]->forward[i].get() != curr)
            continue;
        update[i]->forward[i] = std::move(curr->forward[i]);
    }

    while (level > 0 && !head->forward[level])
        --level;

    node_count--;
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

    const T& at(size_t index) const {
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

    T& operator[](size_t index) {
        return at(index);
    }

    const T& operator[](size_t index) const {return at(index);
    }

    void clear() noexcept {
        head->forward.clear();
        head->forward.resize(max_level + 1);
        level = 0;
        node_count = 0;
    }

    void swap(SkipList& other) noexcept {
        using std::swap;
        swap(head, other.head);
        swap(level, other.level);
        swap(node_count, other.node_count);
        swap(gen, other.gen);
        swap(dis, other.dis);
    }

    bool operator==(const SkipList& other) const {
        if (size() != other.size())
            return false;
        auto it1 = begin();
        auto it2 = other.begin();
        auto end1 = end();
        while (it1 != end1) {
            if (*it1 != *it2)
                return false;
            ++it1;
            ++it2;
        }
        return true;
    }

    class iterator {
        Node* ptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        explicit iterator(Node* node = nullptr) : ptr(node) {}

        T& operator*() const { return ptr->data; }
        T* operator->() const { return &ptr->data; }

        iterator& operator++() {
            if (ptr) ptr = ptr->forward[0].get();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
    };

    // const iterator
    class const_iterator {
        const Node* ptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = const T*;
        using reference         = const T&;

        explicit const_iterator(const Node* node = nullptr) : ptr(node) {}

        const T& operator*() const { return ptr->data; }
        const T* operator->() const { return &ptr->data; }

        const_iterator& operator++() {
            if (ptr) ptr = ptr->forward[0].get();
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const const_iterator& other) const { return ptr != other.ptr; }
    };

    iterator begin() noexcept { return iterator(head->forward[0].get()); }
    iterator end() noexcept { return iterator(nullptr); }
    const_iterator begin() const noexcept { return const_iterator(head->forward[0].get()); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    const_iterator cbegin() const noexcept { return const_iterator(head->forward[0].get()); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
};

#endif // SKIP_LIST_HPP