#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

using namespace std;

struct CaseInsensitiveHash {
    size_t operator()(const string& key) const {
        size_t hash = 0;
        for (char c : key) {
            hash = hash * 31 + tolower(c);
        }
        return hash;
    }
};

struct CaseInsensitiveEqual {
    bool operator()(const string& a, const string& b) const {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); i++) {
            if (tolower(a[i]) != tolower(b[i])) return false;
        }
        return true;
    }
};

class HTTPRequest {
public:
    string raw;
    string method;
    string request_target;
    string path;
    unordered_map<string, string> parameters;        // Filled by routing layer
    unordered_map<string, string> query_parameters;  // Filled by codec
    string version;
    unordered_map<string, string, CaseInsensitiveHash, CaseInsensitiveEqual> headers;
    string body;
};

class HTTPResponse {
public:
    string raw;
    string version;
    uint16_t status_code;
    string reason_phrase;
    unordered_map<string, string, CaseInsensitiveHash, CaseInsensitiveEqual> headers;
    string body;
};

HTTPRequest decode_http_request(const char* raw_request);
HTTPResponse decode_http_response(const char* raw_response);

string encode_http_request(const HTTPRequest &request);
string encode_http_response(const HTTPResponse &response);
