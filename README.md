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
- `src/main.cpp` — Example demonstrating decoding, normalization, and encoding HTTP messages.

---

## Getting Started

### Clone

```sh
git clone https://github.com/bokshi-gh/http-codec.git
cd http-codec
```

### Compile

```sh
g++ -std=c++17 src/main.cpp src/http_codec.cpp src/codec_helpers.cpp -Iinclude -o http_codec_demo
```

### Run

```sh
./http_codec_demo
```

---

## Example Usage

See the full demo in [`src/main.cpp`](src/main.cpp).

---

## Installation

### System-Wide Installation via Remote Installer Script

Run the installer script:

```sh
curl -sSL https://raw.githubusercontent.com/bokshi-gh/http_codec/main/scripts/install.sh | sudo bash
```

> After installation, update the linker cache on Linux:
> ```sh
> sudo ldconfig
> ```

### Using the Library in Your Code

Include the header in your C++ source files:

```cpp
#include <http_codec/http_codec.hpp>
```

Compile your project with:

```sh
g++ main.cpp -lhttp_codec -o your_app
```

---

## Notes

- Assumes well-formed HTTP/1.1 messages; malformed requests may produce undefined behavior.
- Minimal error handling is included for empty request/response lines, missing version, or status code, and HTTP format validation.
- The library does not populate dynamic route parameters; this is the job of your routing layer.

---

## License

This project is licensed under the [MIT License](./LICENSE).  
View the source code on [GitHub](https://github.com/bokshi-gh/http-codec.git).
