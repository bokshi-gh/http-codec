#include "http_codec.hpp"
#include "codec_helpers.hpp"

#include <stdexcept>
#include <regex>

using namespace std;

HTTPRequest decode_http_request(const char* raw_request) {
    HTTPRequest request;
    string raw(raw_request);
    request.raw = raw;

    size_t first_crlf = raw.find("\r\n");
    if (first_crlf == string::npos) return request;

    parse_request_line(request, raw.substr(0, first_crlf));

    size_t header_end = raw.find("\r\n\r\n");
    if (header_end != string::npos) {
        parse_headers(request, raw.substr(first_crlf + 2, header_end - (first_crlf + 2)));
        request.body = (header_end + 4 < raw.size()) ? raw.substr(header_end + 4) : "";
    }

    return request;
}

HTTPResponse decode_http_response(const char* raw_response) {
    HTTPResponse response;
    string raw(raw_response);
    response.raw = raw;

    size_t first_crlf = raw.find("\r\n");
    if (first_crlf == string::npos) return response;

    parse_response_line(response, raw.substr(0, first_crlf));

    size_t header_end = raw.find("\r\n\r\n");
    if (header_end != string::npos) {
        parse_headers(response, raw.substr(first_crlf + 2, header_end - (first_crlf + 2)));
        response.body = (header_end + 4 < raw.size()) ? raw.substr(header_end + 4) : "";
    }

    return response;
}

string encode_http_request(const HTTPRequest &request) {
    if (request.method.empty()) throw invalid_argument("Method empty");
    if (request.request_target.empty()) throw invalid_argument("Target empty");
    if (request.version.empty()) throw invalid_argument("Version empty");

    regex version_regex(R"(HTTP/(1\.[01]|2\.0|3\.0))");
    if (!regex_match(request.version, version_regex))
        throw invalid_argument("Invalid HTTP version format");

    const auto& headers = request.headers;

    string raw = request.method + " " + request.request_target + " " + request.version + "\r\n";

    static unordered_set<string> no_merge = {
        "set-cookie",
        "www-authenticate",
        "authorization"
    };

    for (const auto &h : headers) {
        const string& key = h.first;
        const vector<string>& values = h.second;

        if (no_merge.count(key)) {
            for (const auto &value : values) {
                raw += key + ": " + value + "\r\n";
            }
        } else {
            string merged;
            for (size_t i = 0; i < values.size(); i++) {
                if (i > 0) merged += ", ";
                merged += values[i];
            }
            raw += key + ": " + merged + "\r\n";
        }
    }

    raw += "\r\n";
    raw += request.body;

    return raw;
}

string encode_http_response(const HTTPResponse &response) {
    if (response.version.empty()) throw invalid_argument("Version empty");

    regex version_regex(R"(HTTP/(1\.[01]|2\.0|3\.0))");
    if (!regex_match(response.version, version_regex))
        throw invalid_argument("Invalid HTTP version format");

    if (response.status_code < 100 || response.status_code > 599)
        throw invalid_argument("Invalid status code");

    auto headers = response.headers;

    string raw = response.version + " " + to_string(response.status_code) + " " + response.reason_phrase + "\r\n";

    static unordered_set<string> no_merge = {
        "set-cookie",
        "www-authenticate",
        "authorization"
    };

    for (const auto &h : headers) {
        const string &key = h.first;
        const vector<string> &values = h.second;

        if (no_merge.count(key)) {
            for (const auto &value : values) {
                raw += key + ": " + value + "\r\n";
            }
        } else {
            string merged;
            for (size_t i = 0; i < values.size(); i++) {
                if (i > 0) merged += ", ";
                merged += values[i];
            }
            raw += key + ": " + merged + "\r\n";
        }
    }

    raw += "\r\n";
    raw += response.body;

    return raw;
}
