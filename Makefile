CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -O2 -fPIC

SRCS := src/http_codec.cpp src/codec_helpers.cpp
OBJS := $(SRCS:.cpp=.o)

LIB_NAME := http_codec
STATIC_LIB := lib$(LIB_NAME).a

all: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS)
	ar rcs $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(STATIC_LIB)

.PHONY: all clean
