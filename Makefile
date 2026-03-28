# Makefile for http_codec static library

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -O2 -fPIC

# Sources
SRCS := src/http_codec.cpp src/codec_helpers.cpp

# Object files
OBJS := $(SRCS:.cpp=.o)

# Static library name
STATIC_LIB := libhttp_codec.a

# Installation directories (can override PREFIX)
PREFIX ?= /usr/local
INCLUDE_DIR := $(PREFIX)/include/http_codec
LIB_DIR := $(PREFIX)/lib

# Default target
all: $(STATIC_LIB)

# Build static library
$(STATIC_LIB): $(OBJS)
	ar rcs $@ $^

# Compile .cpp files into .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Install headers and library
install: all
	@echo "Installing headers to $(INCLUDE_DIR)"
	mkdir -p $(INCLUDE_DIR)
	cp -r include/* $(INCLUDE_DIR)/
	@echo "Installing static library to $(LIB_DIR)"
	cp $(STATIC_LIB) $(LIB_DIR)/
	@echo "Done. You may need sudo for system directories."

# Uninstall headers and library
uninstall:
	@echo "Removing headers from $(INCLUDE_DIR)"
	rm -rf $(INCLUDE_DIR)
	@echo "Removing static library from $(LIB_DIR)/$(STATIC_LIB)"
	rm -f $(LIB_DIR)/$(STATIC_LIB)
	@echo "Uninstall complete."

# Clean build files
clean:
	rm -f $(OBJS) $(STATIC_LIB)

# Phony targets
.PHONY: all clean install uninstall
