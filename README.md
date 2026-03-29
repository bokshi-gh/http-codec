# HTTP Codec

A lightweight C++ library for encoding and decoding HTTP/1.1 requests and responses.

It provides structured abstractions (`HTTPRequest`, `HTTPResponse`) and utilities to convert between raw HTTP messages and strongly-typed objects.  

Ideal for building C++ backend services or experimenting with HTTP message handling, or learning low-level HTTP processing in C++.

---

## Features

- Encode `HTTPRequest` / `HTTPResponse` into raw HTTP strings.
- Decode raw HTTP strings into structured `HTTPRequest` / `HTTPResponse`.
- Compatible with HTTP/1.1 request and response formats.

---

## Project Structure

The project includes the following files:

- `include/http_codec.hpp` — Class definitions and function declarations.
- `include/codec_helpers.hpp` — Internal helpers for parsing request/response lines, headers, and query parameters.
- `src/http_codec.cpp` — Implementation of encoding and decoding functions.
- `src/codec_helpers.cpp` — Parsing helpers implementation.
- `examples/http_codec_demo.cpp` — Example demonstrating decoding, normalization, and encoding HTTP messages.

---

## Example Usage

See the full demo in [`examples/http_codec_demo.cpp`](examples/http_codec_demo.cpp).

---

## Notes

- Assumes well-formed HTTP/1.1 messages; malformed requests may produce undefined behavior.
- Minimal error handling is included for empty request/response lines, missing version, or status code, and HTTP format validation.
- The library does not populate dynamic route parameters; this is the job of your routing layer.

---

## License

This project is licensed under the [MIT License](./LICENSE).  
View the source code on [GitHub](https://github.com/bokshi-gh/http-codec.git).
