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

    istringstream iss(request_line);
    iss >> request.method >> request.request_target >> request.version;

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

    istringstream iss(status_line);
    iss >> response.version >> response.status_code;

    getline(iss, response.reason_phrase);
    if (!response.reason_phrase.empty() && response.reason_phrase[0] == ' ')
        response.reason_phrase.erase(0, 1);

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
    if (request.method.empty() ||
        request.request_target.empty() ||
        request.version.empty()) {
        return "";
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
    if (response.version.empty()) return "";

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
