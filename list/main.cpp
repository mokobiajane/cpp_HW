
#include "skip_list.hpp"
#include <iostream>
#include <cassert>

int main() {
    SkipList<int> sl;

    std::cout << "Inserting: 10, 5, 20, 15, 30\n";
    sl.insert(10);
    sl.insert(5);
    sl.insert(20);
    sl.insert(15);
    sl.insert(30);

    assert(sl.find(10));
    assert(!sl.find(100));
    sl.erase(10);
    assert(!sl.find(10));

    std::cout << "Elements: ";
    for (auto it = sl.begin(); it != sl.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";

    assert(sl.at(0) == 5);
    assert(sl.at(1) == 15);
    assert(sl.at(2) == 20);
    assert(sl.at(3) == 30);

    std::cout << "All tests passed.\n";
    return 0;
}