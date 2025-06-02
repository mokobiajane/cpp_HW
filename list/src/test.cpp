

#include "skip_list.hpp"
#include <gtest/gtest.h>

TEST(SkipListTest, InsertAndFind) {
    SkipList<int> list;
    list.insert(5);
    list.insert(10);
    list.insert(1);

    EXPECT_TRUE(list.find(10));
    EXPECT_FALSE(list.find(100));
    EXPECT_TRUE(list.find(1));
}

TEST(SkipListTest, Erase) {
    SkipList<int> list;
    list.insert(10);
    list.insert(20);
    EXPECT_TRUE(list.erase(10));
    EXPECT_FALSE(list.erase(10));
    EXPECT_FALSE(list.find(10));
}

TEST(SkipListTest, AtAccess) {
    SkipList<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);
    EXPECT_EQ(list.at(0), 1);
    EXPECT_EQ(list.at(2), 3);
    EXPECT_THROW(list.at(5), std::out_of_range);
}

TEST(SkipListTest, Iterator) {
    SkipList<int> list;
    list.insert(3);
    list.insert(1);
    list.insert(2);

    std::vector<int> values;
    for (int v : list)
        values.push_back(v);

    EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}