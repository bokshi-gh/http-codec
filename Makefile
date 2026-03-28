CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -O2 -fPIC

SRCS := src/http_codec.cpp src/codec_helpers.cpp
OBJS := $(SRCS:.cpp=.o)

LIB_NAME := http_codec
STATIC_LIB := lib$(LIB_NAME).a

PREFIX ?= /usr/local
INCLUDE_DIR := $(PREFIX)/include/$(LIB_NAME)
LIB_DIR := $(PREFIX)/lib

VERSION ?= 1.0.0
RELEASE_NAME := $(LIB_NAME)-$(VERSION)
RELEASE_DIR := release/$(RELEASE_NAME)

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

release: all
	rm -rf release
	mkdir -p $(RELEASE_DIR)
	cp -r include $(RELEASE_DIR)/
	mkdir -p $(RELEASE_DIR)/lib
	cp $(STATIC_LIB) $(RELEASE_DIR)/lib/
	cp README.md $(RELEASE_DIR)/ 2>/dev/null || true
	cp INSTALLATION.md $(RELEASE_DIR)/ 2>/dev/null || true
	cp LICENSE $(RELEASE_DIR)/ 2>/dev/null || true
	cd release && zip -r $(RELEASE_NAME).zip $(RELEASE_NAME)

clean:
	rm -f $(OBJS) $(STATIC_LIB)

.PHONY: all install uninstall release clean
