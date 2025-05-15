# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Ilib -g -DDEBUG -I$(GTEST_INC)
DEBUG ?= 1
GTEST_INC = /opt/homebrew/include
GTEST_LIB = /opt/homebrew/lib
LDFLAGS = -L$(GTEST_LIB) -lgtest -lgtest_main -pthread

ifeq ($(DEBUG), 1)
	CXXFLAGS = -std=c++20 -Wall -Ilib -g -DDEBUG -I$(GTEST_INC)
else
	CXXFLAGS = -std=c++20 -Wall -Ilib -O2 -I$(GTEST_INC)
endif


# Source files
SRC = main.cpp lib/parser.cpp lib/utils.cpp
OBJ = $(patsubst %.cpp, build/%.o, $(SRC))

TEST_SRC = tests/test_parser.cpp tests/test_utils.cpp
TEST_OBJ = $(patsubst %.cpp, build/%.o, $(TEST_SRC))
TEST_TARGET = build/test_runner

# Output binary
TARGET = app

# Default target
all: $(TARGET)

# Build rule
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TEST_OBJ) lib/parser.cpp lib/utils.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) lib/parser.cpp lib/utils.cpp $(LDFLAGS)

# Compile source files into object files
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run-test: test
	./$(TEST_TARGET)

# Clean up build files
clean:
	rm -rf build $(TARGET)
