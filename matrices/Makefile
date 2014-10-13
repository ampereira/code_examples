################################################################################
# Makefile for Simple Performance Analyser
#
# by André Pereira (LIP-Minho)
################################################################################

SHELL = /bin/sh

DEFINES = -Dextname

BIN_NAME = matrix_mult

CXX        = g++-4.9
LD         = g++-4.9

CXXFLAGS   = -Wall -Wextra --std=c++11 -lspa

ifeq ($(AVX),yes)
	CXXFLAGS += -mavx -DAVX
else
	ifeq ($(SSE),yes)
		CXXFLAGS += -msse4.1 -DSSE
	endif
endif

ifeq ($(DEBUG),yes)
	CXXFLAGS += -ggdb3
else
#CXXFLAGS += -O3
endif

SPA_LIB=/Users/andre/PhD/spa
COMMON_HEADERS=/Users/andre/PhD/code_examples/common

INCLUDES = -I$(SPA_LIB)/src -I$(COMMON_HEADERS) -L$(SPA_LIB)/lib

################################################################################
# Control awesome stuff
################################################################################

SRC_DIR = src
BIN_DIR = bin
BUILD_DIR = build
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))
DEPS = $(patsubst build/%.o,build/%.d,$(OBJ))
BIN = $(BIN_NAME)

vpath %.cpp $(SRC_DIR)

################################################################################
# Rules
################################################################################

.DEFAULT_GOAL = all

$(BUILD_DIR)/%.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(BIN_DIR)/$(BIN_NAME): $(DEPS) $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJ)

checkdirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

all: checkdirs $(BIN_DIR)/$(BIN_NAME)

clean:
	rm -f $(BUILD_DIR)/* $(BIN_DIR)/* 