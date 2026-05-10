CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -g
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
CLI_BIN := $(BUILD_DIR)/pyjit
TEST_BIN := $(BUILD_DIR)/pyjit_tests

FRONTEND_SRCS := src/frontend/lexer.cpp
CLI_SRCS := src/cli/main.cpp
TEST_SRCS := tests/test_main.cpp tests/lexer_tests.cpp

.PHONY: all test clean

all: $(CLI_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLI_BIN): $(CLI_SRCS) $(FRONTEND_SRCS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(TEST_BIN): $(TEST_SRCS) $(FRONTEND_SRCS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

test: $(TEST_BIN)
	$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)
