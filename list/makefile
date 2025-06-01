CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = main
TEST_TARGET = test

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: test.cpp
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) test.cpp -lgtest -lgtest_main -pthread
	./$(TEST_TARGET)

docs:
	doxygen Doxyfile

clean:
	rm -f $(TARGET) $(TEST_TARGET)
