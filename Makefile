# This Makefile is intentionally a scaffold.
# It names the build entry points now so later milestones can fill in real recipes.

CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -g
CPPFLAGS ?= -Iinclude

BUILD_DIR := build

.PHONY: all test clean

all:
	@printf 'Build scaffold only: no compilation rules are defined yet.\n'

test:
	@printf 'Test scaffold only: no executable tests are defined yet.\n'

clean:
	@printf 'Clean scaffold only: no build artifacts are expected yet.\n'
