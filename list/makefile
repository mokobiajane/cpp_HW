CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Iinclude
SRC_DIR = src
INCLUDE_DIR = include
TARGET = main
TEST_TARGET = test

# Source files
SRCS = $(SRC_DIR)/main.cpp
TEST_SRCS = $(SRC_DIR)/test.cpp

# Default target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $^ -lgtest -lgtest_main -pthread
	./$(TEST_TARGET)

docs:
	doxygen Doxyfile

clean:
	rm -f $(TARGET) $(TEST_TARGET)