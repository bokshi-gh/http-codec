#pragma once

#include "http_codec.hpp"

#include <string>
#include <regex>
#include <cctype>
#include <cstdlib>


using namespace std;

// Request
void parse_request_target(HTTPRequest& req);
void normalize_request_target(HTTPRequest& req);
void parse_request_line(HTTPRequest& req, const string& line);

// Response
void parse_response_line(HTTPResponse& res, const string& line);

// Headers (generic for request/response)
template<typename T>
void parse_headers(T& object, const string& headers_block);

// Body (generic for request/response)
template<typename U>
void parse_body(U& object, const string& raw, size_t body_start);

// Body (generic for request/response);
template<typename V>
string encode_body(const V &message);
