#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <memory>
#include <vector>
#include <random>
#include <stdexcept>

/**
 * @brief A templated skip list implementation with support for iteration.
 * 
 * @tparam T Type of elements stored in the skip list.
 */
template<typename T>
class SkipList {
private:
    /**
     * @brief A node within the skip list.
     */
    struct Node {
        T data;  ///< The value stored in the node.
        std::vector<std::unique_ptr<Node>> forward; ///< Forward pointers to next nodes.

        /**
         * @brief Construct a new Node.
         * 
         * @param value The value to store.
         * @param level The level of the node.
         */
        Node(const T& value, int level) : data(value), forward(level + 1) {}
    };

    std::unique_ptr<Node> head; ///< Head node of the skip list.
    int level; ///< Current maximum level in the skip list.
    static constexpr int max_level = 16; ///< Maximum level allowed.
    std::mt19937 gen; ///< Random number generator.
    std::uniform_real_distribution<> dis; ///< Uniform distribution for random level generation.

    /**
     * @brief Generates a random level for a new node.
     * 
     * @return The randomly generated level.
     */
    int randomLevel() {
        int lvl = 0;
        while (dis(gen) < 0.5 && lvl < max_level)
            lvl++;
        return lvl;
    }

public:
    /**
     * @brief Constructs an empty skip list.
     */
    SkipList() : level(0), gen(std::random_device{}()), dis(0.0, 1.0) {
        head = std::make_unique<Node>(T{}, max_level);
    }

    /**
     * @brief Searches for a value in the skip list.
     * 
     * @param value The value to search for.
     * @return true if found, false otherwise.
     * 
     * @code
     * SkipList<int> sl;
     * sl.insert(10);
     * assert(sl.find(10)); // true
     * @endcode
     */
    bool find(const T& value) const {
        Node* curr = head.get();
        for (int i = level; i >= 0; i--) {
            while (curr->forward[i] && curr->forward[i]->data < value)
                curr = curr->forward[i].get();
        }
        curr = curr->forward[0].get();
        return curr && curr->data == value;
    }

    /**
     * @brief Inserts a value into the skip list.
     * 
     * @param value The value to insert.
     * 
     * @note Duplicate values are not inserted.
     * 
     * @code
     * SkipList<int> sl;
     * sl.insert(42);
     * @endcode
     */
    void insert(const T& value) {
        std::vector<Node*> update(max_level + 1, nullptr);
        Node* curr = head.get();

        for (int i = level; i >= 0; i--) {
            while (curr->forward[i] && curr->forward[i]->data < value)
                curr = curr->forward[i].get();
            update[i] = curr;
        }

        curr = curr->forward[0].get();

        if (curr && curr->data == value)
            return; // value already exists

        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level + 1; i <= newLevel; i++) {
                update[i] = head.get();
            }
            level = newLevel;
        }

        auto newNode = std::make_unique<Node>(value, newLevel);
        Node* newNodeRaw = newNode.get(); // Track raw pointer for ownership logic

        for (int i = 0; i <= newLevel; i++) {
            newNode->forward[i] = std::move(update[i]->forward[i]);
            update[i]->forward[i] = nullptr; // Ensure overwrite
        }

        for (int i = 0; i <= newLevel; i++) {
            update[i]->forward[i] = std::move(newNode);
            if (i != newLevel)
                newNode = std::make_unique<Node>(*newNodeRaw); // reuse same node pointer
        }
    }

    /**
     * @brief Removes a value from the skip list.
     * 
     * @param value The value to remove.
     * @return true if the value was found and removed, false otherwise.
     * 
     * @code
     * sl.erase(10);
     * @endcode
     */
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
            return false;

        for (int i = 0; i <= level; i++) {
            if (update[i]->forward[i].get() != curr)
                break;
            update[i]->forward[i] = std::move(curr->forward[i]);
        }

        while (level > 0 && !head->forward[level])
            level--;

        return true;
    }

    /**
     * @brief Accesses the element at the specified index.
     * 
     * @param index The index (0-based).
     * @return Reference to the value at the index.
     * 
     * @throws std::out_of_range if the index is out of bounds.
     */
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

    /**
     * @brief Iterator class for SkipList.
     * 
     * This is a forward iterator that allows range-based for loop support.
     * It iterates over the base level (level 0) of the skip list.
     * 
     * @note Iterators become invalid if elements are inserted or erased during iteration.
     * 
     * @code
     * for (auto it = sl.begin(); it != sl.end(); ++it) {
     *     std::cout << *it << " ";
     * }
     * @endcode
     */
    class iterator {
        Node* ptr;

    public:
        /// Construct an iterator from a node pointer
        explicit iterator(Node* node = nullptr) : ptr(node) {}

        /// Dereference the iterator
        T& operator*() const {
            if (!ptr) throw std::runtime_error("Dereferencing end() iterator");
            return ptr->data;
        }

        /// Pre-increment
        iterator& operator++() {
            if (ptr) ptr = ptr->forward[0].get();
            return *this;
        }

        /// Inequality comparison
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }

        /// Equality comparison
        bool operator==(const iterator& other) const { return ptr == other.ptr; }
    };

    /**
     * @brief Returns an iterator to the first element.
     * 
     * @return iterator to the first node.
     * 
     * @code
     * for (int x : sl) {
     *     std::cout << x << " ";
     * }
     * @endcode
     */
    iterator begin() { return iterator(head->forward[0].get()); }

    /**
     * @brief Returns an iterator to the end of the list.
     * 
     * @return iterator to nullptr.
     */
    iterator end() { return iterator(nullptr); }
};

#endif // SKIP_LIST_HPP