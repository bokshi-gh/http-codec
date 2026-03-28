#include "http_codec.hpp"
#include <sstream>

using namespace std;

static void parse_request_target(HTTPRequest& req) {
    const string& target = req.request_target;

    size_t qpos = target.find('?');

    if (qpos == string::npos) {
        req.path = target;
        return;
    }

    req.path = target.substr(0, qpos);

    string query = target.substr(qpos + 1);

    size_t start = 0;
    while (start < query.size()) {
        size_t eq = query.find('=', start);
        size_t amp = query.find('&', start);

        if (amp == string::npos) amp = query.size();

        string key = query.substr(start,
            (eq != string::npos && eq < amp) ? eq - start : amp - start);

        string val = (eq != string::npos && eq < amp)
            ? query.substr(eq + 1, amp - eq - 1)
            : "";

        req.query_parameters[key] = val;

        start = amp + 1;
    }
}

HTTPRequest decode_http_request(const char* raw_request) {
    HTTPRequest request;

    string raw(raw_request);
    request.raw = raw;

    size_t first_crlf = raw.find("\r\n");
    if (first_crlf == string::npos) return request;

    string request_line = raw.substr(0, first_crlf);

    if (request_line.empty()) {
        throw std::runtime_error("Empty request line received");
    }

    std::istringstream iss(request_line);

    // Try to extract all three parts
    if (!(iss >> request.method >> request.request_target >> request.version)) {
        throw std::runtime_error("Incomplete request line: expected METHOD TARGET VERSION");
    }

    // Optional: Check for extra data (RFC compliance)
    // If the string has 4 words instead of 3, this catches it
    std::string extra;
    if (iss >> extra) {
        throw std::runtime_error("Malformed request: unexpected extra data after version");
    }
    
    parse_request_target(request);

    size_t header_end = raw.find("\r\n\r\n");
    if (header_end == string::npos) return request;

    size_t headers_start = first_crlf + 2;
    string headers_block = raw.substr(headers_start, header_end - headers_start);

    size_t pos = 0;
    while (pos < headers_block.size()) {
        size_t end = headers_block.find("\r\n", pos);
        if (end == string::npos) end = headers_block.size();

        string header = headers_block.substr(pos, end - pos);

        size_t colon = header.find(':');
        if (colon != string::npos) {
            string key = header.substr(0, colon);

            size_t value_start = colon + 1;
            if (value_start < header.size() && header[value_start] == ' ')
                value_start++;

            string value = header.substr(value_start);

            request.headers[key] = value;
        }

        pos = end + 2;
    }

    request.body = raw.substr(header_end + 4);

    return request;
}

HTTPResponse decode_http_response(const char* raw_response) {
    HTTPResponse response;

    string raw(raw_response);
    response.raw = raw;

    size_t first_crlf = raw.find("\r\n");
    if (first_crlf == string::npos) return response;

    string status_line = raw.substr(0, first_crlf);

    if (status_line.empty()) {
        throw std::runtime_error("Empty status line received");
    }

    std::istringstream iss(status_line);

    // Extract Version (e.g., HTTP/1.1)
    if (!(iss >> response.version)) {
        throw std::runtime_error("Response Error: Missing HTTP version");
    }

    // Extract Status Code (e.g., 200)
    if (!(iss >> response.status_code)) {
        throw std::runtime_error("Response Error: Missing status code");
    }

    // Extract the rest of the line as the Reason Phrase (e.g., OK)
    // getline grabs everything left in the stream
    std::getline(iss, response.reason_phrase);

    // Clean up the leading space left by the '>>' operator
    if (!response.reason_phrase.empty() && response.reason_phrase[0] == ' ') {
        response.reason_phrase.erase(0, 1);
    }
    
    size_t header_end = raw.find("\r\n\r\n");
    if (header_end == string::npos) return response;

    size_t headers_start = first_crlf + 2;
    string headers_block = raw.substr(headers_start, header_end - headers_start);

    size_t pos = 0;
    while (pos < headers_block.size()) {
        size_t end = headers_block.find("\r\n", pos);
        if (end == string::npos) end = headers_block.size();

        string header = headers_block.substr(pos, end - pos);

        size_t colon = header.find(':');
        if (colon != string::npos) {
            string key = header.substr(0, colon);

            size_t value_start = colon + 1;
            if (value_start < header.size() && header[value_start] == ' ')
                value_start++;

            string value = header.substr(value_start);

            response.headers[key] = value;
        }

        pos = end + 2;
    }

    response.body = raw.substr(header_end + 4);

    return response;
}

string encode_http_request(const HTTPRequest &request) {
    if (req.method.empty()) {
        throw std::runtime_error("Cannot build request line: Method is empty");
    }
    if (req.request_target.empty()) {
        throw std::runtime_error("Cannot build request line: Target is empty");
    }
    if (req.version.empty()) {
        throw std::runtime_error("Cannot build request line: Version is empty");
    }

    string raw = request.method + " " +
                 request.request_target + " " +
                 request.version + "\r\n";

    for (const auto &h : request.headers) {
        raw += h.first + ": " + h.second + "\r\n";
    }

    raw += "\r\n";
    raw += request.body;

    return raw;
}

string encode_http_response(const HTTPResponse &response) {
    if (response.version.empty()) {
        throw runtime_error("Cannot build response line: Version is empty");
    }
    if (!response.status_code.has_value()) {
        throw runtime_error("Cannot build response line: Status Code is empty");
    }

    string raw = response.version + " " +
                 to_string(response.status_code) + " " +
                 response.reason_phrase + "\r\n";

    for (const auto &h : response.headers) {
        raw += h.first + ": " + h.second + "\r\n";
    }

    raw += "\r\n";
    raw += response.body;

    return raw;
}
