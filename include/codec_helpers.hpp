#pragma once
#include "http_codec.hpp"
#include <string>
#include <regex>

using namespace std;

// Request
void parse_request_target(HTTPRequest& req);
void parse_request_line(HTTPRequest& req, const string& line);

// Response
void parse_response_line(HTTPResponse& res, const string& line);

// Headers (generic for request/response)
template<typename T>
void parse_headers(T& msg, const string& headers_block);
