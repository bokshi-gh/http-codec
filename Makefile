CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -O2 -fPIC

SRCS := src/http_codec.cpp src/codec_helpers.cpp
OBJS := $(SRCS:.cpp=.o)

LIB_NAME := http_codec
STATIC_LIB := lib$(LIB_NAME).a

PREFIX ?= /usr/local
INCLUDE_DIR := $(PREFIX)/include
LIB_DIR := $(PREFIX)/lib

all: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS)
	ar rcs $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: all
	mkdir -p $(INCLUDE_DIR)
	cp -r include/* $(INCLUDE_DIR)/
	mkdir -p $(LIB_DIR)
	cp $(STATIC_LIB) $(LIB_DIR)/

uninstall:
	rm -rf $(INCLUDE_DIR)
	rm -f $(LIB_DIR)/$(STATIC_LIB)

clean:
	rm -f $(OBJS) $(STATIC_LIB)

.PHONY: all install uninstall release clean
