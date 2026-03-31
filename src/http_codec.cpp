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
        
        // --- RFC: GET and HEAD usually do not have a body ---
        if (request.method != "GET" && request.method != "HEAD") {
            parse_body(request, raw, header_end + 4);
        } 
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
        parse_body(response, raw, header_end + 4);
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

    string raw = request.method + " " + request.request_target + " " + request.version + "\r\n";

    for (auto &h : request.headers) raw += h.first + ": " + h.second + "\r\n";
    raw += "\r\n";

    raw += encode_body(request);
    return raw;
}

string encode_http_response(const HTTPResponse &response) {
    if (response.version.empty()) throw invalid_argument("Version empty");

    regex version_regex(R"(HTTP/(1\.[01]|2\.0|3\.0))");
    if (!regex_match(response.version, version_regex))
        throw invalid_argument("Invalid HTTP version format");

    if (response.status_code < 100 || response.status_code > 599)
        throw invalid_argument("Invalid status code");

    string raw = response.version + " " + to_string(response.status_code) + " " + response.reason_phrase + "\r\n";

    if (!response.body.empty()) {
        headers["Content-Length"] = to_string(response.body.size());  // always override
        // headers.erase("Transfer-Encoding"); // optional, if you prefer strict Content-Length
    }

    for (auto &h : response.headers) raw += h.first + ": " + h.second + "\r\n";
    raw += "\r\n";

    raw += response.body;
    
    return raw;
}
