#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

using namespace std;

class HTTPRequest {
public:
    string raw;
    string method;

    // Raw request target from HTTP (e.g. "/users/?id=1")
    string request_target;

    string path;

    // Populated by the routing layer (not by the HTTP codec)
    // The codec does not know route patterns, so it cannot extract dynamic parameters
    unordered_map<string, string> dynamic_parameters;

    // Parsed from query string (?a=1&b=2)
    unordered_map<string, string> query_parameters;

    string version;
    unordered_map<string, string> headers;
    string body;
};

class HTTPResponse {
public:
    string raw;
    string version;
    uint16_t status_code;
    string reason_phrase;
    unordered_map<string, string> headers;
    string body;
};


HTTPRequest decode_http_request(const char *raw_request);
HTTPResponse decode_http_response(const char *raw_response);

string encode_http_request(const HTTPRequest &request);
string encode_http_response(const HTTPResponse &response);
