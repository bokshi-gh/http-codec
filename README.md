# HTTP Codec

A lightweight C++ library for encoding and decoding HTTP/1.1 requests and responses.

It provides structured abstractions (HTTPRequest, HTTPResponse) and utilities to convert between raw HTTP messages and strongly-typed objects.

## Features

- Encode `HTTPRequest` / `HTTPResponse` into raw HTTP strings.
- Decode raw HTTP strings into structured `HTTPRequest` / `HTTPResponse`.
- Handles optional headers and body safely.
- Compatible with HTTP/1.1 request and response formats.

## Project Structure

The project includes the following files:

- `include/http_codec.hpp` - Class definitions and function declarations.
- `src/http_codec.cpp` - Implementation of encoding and decoding functions.
- `src/main.cpp` - Example code demonstrating how to build, encode, and decode HTTP requests and responses.

## Getting Started

Compile

```sh
g++ -std=c++17 src/main.cpp src/http_codec.cpp -Iinclude -o http_codec_demo
```

Run

```sh
./http_codec_demo
```

## Notes

- This library assumes well-formed HTTP/1.1 messages
- Error handling and validation are minimal (by design)

## License

This project is licensed under the [MIT License](./LICENSE).  
View the source code on [GitHub](https://github.com/bokshi-gh/http-codec.git).
