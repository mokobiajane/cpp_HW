#include "skip_list.hpp"
#include <iostream>
#include <cassert>

int main() {
    SkipList<int> sl;

    sl.insert(10);
    sl.insert(5);
    sl.insert(20);
    sl.insert(15);
    sl.insert(30);

    assert(sl.find(10));
    assert(sl.find(5));
    assert(sl.find(20));
    assert(sl.find(15));
    assert(sl.find(30));
    assert(!sl.find(100));

    std::cout << "Elements in skip list:" << std::endl;
    for (int v : sl) {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    bool erased = sl.erase(20);
    assert(erased);
    assert(!sl.find(20));

    erased = sl.erase(100);
    assert(!erased);

    std::cout << "After erasing 20:" << std::endl;
    for (int v : sl) {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    return 0;
}