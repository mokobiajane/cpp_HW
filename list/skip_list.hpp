#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <memory>
#include <stdexcept>
#include <iterator>

/**
 * @file skip_list.hpp
 * @brief A templated, STL-style skip list with forward iterator support.
 * 
 * This class provides a skip list data structure offering average-case 
 * O(log n) complexity for insert, erase, and find operations. It supports 
 * range-based for loops through its forward iterator.
 * 
 * @tparam T The type of elements stored in the skip list.
 * 
 * ### Example
 * @code
 * #include "skip_list.hpp"
 * #include <iostream>
 * 
 * int main() {
 *     SkipList<int> sl;
 *     sl.insert(10);
 *     sl.insert(5);
 *     sl.insert(20);
 * 
 *     for (int val : sl) {
 *         std::cout << val << " ";
 *     }
 *     std::cout << std::endl;
 * }
 * @endcode
 */
template<typename T>
class SkipList {
private:
    static constexpr float PROBABILITY = 0.5;
    static constexpr int MAX_LEVEL = 16;

    struct Node {
        T data;
        std::vector<Node*> forward;

        Node(const T& val, int level) : data(val), forward(level + 1, nullptr) {}
    };

    Node* head;
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
    /**
     * @brief Constructs an empty skip list.
     */
    SkipList() : size_(0), level(0), rng(std::random_device{}()), dist(0.0, 1.0) {
        head = new Node(T{}, MAX_LEVEL);
    }

    /**
     * @brief Destructor that frees all nodes.
     */
    ~SkipList() {
        clear();
        delete head;
    }

    /**
     * @brief Inserts a value into the skip list.
     * 
     * Duplicate values are ignored.
     * 
     * @param value The value to insert.
     * 
     * ### Example
     * @code
     * sl.insert(42);
     * @endcode
     */
    void insert(const T& value) {
        std::vector<Node*> update(MAX_LEVEL + 1);
        Node* curr = head;

        for (int i = level; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->data < value)
                curr = curr->forward[i];
            update[i] = curr;
        }

        curr = curr->forward[0];

        if (curr && curr->data == value)
            return; // No duplicates

        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level + 1; i <= newLevel; ++i)
                update[i] = head;
            level = newLevel;
        }

        Node* newNode = new Node(value, newLevel);
        for (int i = 0; i <= newLevel; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
        ++size_;
    }

    /**
     * @brief Removes a value from the skip list.
     * 
     * @param value The value to remove.
     * @return true if the value was removed, false if it was not found.
     * 
     * ### Example
     * @code
     * bool success = sl.erase(10);
     * @endcode
     */
    bool erase(const T& value) {
        std::vector<Node*> update(MAX_LEVEL + 1);
        Node* curr = head;

        for (int i = level; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->data < value)
                curr = curr->forward[i];
            update[i] = curr;
        }

        curr = curr->forward[0];
        if (!curr || curr->data != value)
            return false;

        for (int i = 0; i <= level; ++i) {
            if (update[i]->forward[i] != curr)
                break;
            update[i]->forward[i] = curr->forward[i];
        }

        delete curr;

        while (level > 0 && !head->forward[level])
            --level;

        --size_;
        return true;
    }

    /**
     * @brief Checks if the value exists in the skip list.
     * 
     * @param value The value to search for.
     * @return true if the value exists, false otherwise.
     * 
     * ### Example
     * @code
     * if (sl.find(15)) { std::cout << "Found\n"; }
     * @endcode
     */
    bool find(const T& value) const {
        Node* curr = head;
        for (int i = level; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->data < value)
                curr = curr->forward[i];
        }
        curr = curr->forward[0];
        return curr && curr->data == value;
    }

    /**
     * @brief Access element by index (zero-based).
     * 
     * @param index The index of the element.
     * @return A reference to the element at the index.
     * @throws std::out_of_range if the index is invalid.
     * 
     * ### Example
     * @code
     * int val = sl.at(2);
     * @endcode
     */
    T& at(size_t index) {
        if (index >= size_)
            throw std::out_of_range("Index out of range");

        Node* curr = head->forward[0];
        for (size_t i = 0; i < index; ++i)
            curr = curr->forward[0];
        return curr->data;
    }

    /**
     * @brief Returns the number of elements in the list.
     * @return Size of the list.
     */
    size_t size() const { return size_; }

    /**
     * @brief Checks whether the list is empty.
     * @return true if the list is empty.
     */
    bool empty() const { return size_ == 0; }

    /**
     * @brief Removes all elements from the list.
     * 
     * ### Example
     * @code
     * sl.clear();
     * assert(sl.empty());
     * @endcode
     */
    void clear() {
        Node* curr = head->forward[0];
        while (curr) {
            Node* next = curr->forward[0];
            delete curr;
            curr = next;
        }
        for (int i = 0; i <= MAX_LEVEL; ++i)
            head->forward[i] = nullptr;
        level = 0;
        size_ = 0;
    }

    /**
     * @brief Forward iterator for SkipList<T>.
     * 
     * Supports range-based for loops and STL-style iteration.
     */
   /**
     * @brief Forward iterator for the SkipList.
     *
     * This iterator supports standard operations such as dereferencing and increment,
     * and is compatible with range-based for loops.
     *
     * @tparam T The type of element stored in the skip list.
     *
     * ### Example (Manual Iteration)
     * @code
     * SkipList<int> sl;
     * sl.insert(1);
     * sl.insert(3);
     * sl.insert(2);
     *
     * for (SkipList<int>::iterator it = sl.begin(); it != sl.end(); ++it) {
     *     std::cout << *it << " ";
     * }
     * @endcode
     */
    class iterator {
        Node* ptr; ///< Internal pointer to the current node

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using reference         = T&;
        using pointer           = T*;

        /**
         * @brief Constructs an iterator pointing to a given node.
         * @param p A pointer to the node the iterator should point to.
         */
        explicit iterator(Node* p) : ptr(p) {}

        /**
         * @brief Dereference operator.
         * @return A reference to the current element.
         *
         * ### Example
         * @code
         * T val = *it;
         * @endcode
         */
        T& operator*() { return ptr->data; }

        /**
         * @brief Arrow operator.
         * @return A pointer to the current element.
         *
         * ### Example
         * @code
         * auto len = it->length(); // assuming T has a length() method
         * @endcode
         */
        T* operator->() { return &(ptr->data); }

        /**
         * @brief Pre-increment operator.
         * @return Reference to the incremented iterator.
         *
         * ### Example
         * @code
         * ++it;
         * @endcode
         */
        iterator& operator++() {
            if (ptr) ptr = ptr->forward[0];
            return *this;
        }

        /**
         * @brief Equality operator.
         * @param other The iterator to compare with.
         * @return true if both iterators point to the same node.
         */
        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }

        /**
         * @brief Inequality operator.
         * @param other The iterator to compare with.
         * @return true if the iterators point to different nodes.
         */
        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
    };

    /**
     * @brief Returns an iterator to the first element.
     * @return An iterator to the beginning.
     * 
     * ### Example
     * @code
     * for (auto it = sl.begin(); it != sl.end(); ++it) {
     *     std::cout << *it << " ";
     * }
     * @endcode
     */
    iterator begin() { return iterator(head->forward[0]); }

    /**
     * @brief Returns an iterator to the past-the-end element.
     * @return An iterator to the end.
     */
    iterator end() { return iterator(nullptr); }
};

#endif // SKIP_LIST_HPP