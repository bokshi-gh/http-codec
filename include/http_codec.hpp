#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include <optional>

using namespace std;

class HTTPRequest {
public:
    string raw;
    string method;
    string request_target;
    string path;
    unordered_map<string, string> dynamic_parameters; // Filled by routing layer
    unordered_map<string, string> query_parameters;   // Filled by codec
    string version;
    unordered_map<string, string> headers;
    string body;
};

class HTTPResponse {
public:
    string raw;
    string version;
    optional<uint16_t> status_code;
    string reason_phrase;
    unordered_map<string, string> headers;
    string body;
};

// Decode/Encode
HTTPRequest decode_http_request(const char* raw_request);
HTTPResponse decode_http_response(const char* raw_response);

string encode_http_request(const HTTPRequest &request);
string encode_http_response(const HTTPResponse &response);
