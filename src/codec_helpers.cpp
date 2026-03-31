#include "codec_helpers.hpp"

#include <sstream>
#include <stdexcept>

using namespace std;

void normalize_request_target(HTTPRequest& req) {
    while (req.request_target.size() > 1 && req.request_target.back() == '/') {
        req.request_target.pop_back();
    }
}

void parse_request_target(HTTPRequest& req) {
    normalize_request_target(req);

    const string& target = req.request_target;
    size_t qpos = target.find('?');
    req.path = (qpos == string::npos) ? target : target.substr(0, qpos);

    if (qpos == string::npos) return;

    string query = target.substr(qpos + 1);
    size_t start = 0;

    while (start < query.size()) {
        size_t eq = query.find('=', start);
        size_t amp = query.find('&', start);
        if (amp == string::npos) amp = query.size();

        string key = query.substr(start, (eq != string::npos && eq < amp) ? eq - start : amp - start);
        string val = (eq != string::npos && eq < amp) ? query.substr(eq + 1, amp - eq - 1) : "";
        req.query_parameters[key] = val;

        start = amp + 1;
    }
}

void parse_request_line(HTTPRequest& req, const string& line) {
    if (line.empty()) throw invalid_argument("Empty request line");
    istringstream iss(line);
    if (!(iss >> req.method >> req.request_target >> req.version))
        throw invalid_argument("Incomplete request line");

    string extra;
    if (iss >> extra) throw invalid_argument("Unexpected extra data in request line");

    regex version_regex(R"(HTTP/(1\.[01]|2\.0|3\.0))");
    if (!regex_match(req.version, version_regex))
        throw invalid_argument("Invalid HTTP version format");

    parse_request_target(req);
}

void parse_response_line(HTTPResponse& res, const string& line) {
    if (line.empty()) throw invalid_argument("Empty response line");
    istringstream iss(line);
    if (!(iss >> res.version)) throw invalid_argument("Missing HTTP version");
    if (!(iss >> res.status_code)) throw invalid_argument("Missing status code");

    regex version_regex(R"(HTTP/(1\.[01]|2\.0|3\.0))");
    if (!regex_match(res.version, version_regex))
        throw invalid_argument("Invalid HTTP version format");

    if (res.status_code < 100 || res.status_code > 599)
        throw invalid_argument("Invalid HTTP status code");

    getline(iss, res.reason_phrase);
    if (!res.reason_phrase.empty() && res.reason_phrase[0] == ' ')
        res.reason_phrase.erase(0, 1);
}

template<typename T>
void parse_headers(T& object, const string& headers_block) {
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
            object.headers[key] = value;
        }

        pos = end + 2;
    }
}

template void parse_headers<HTTPRequest>(HTTPRequest&, const string&);
template void parse_headers<HTTPResponse>(HTTPResponse&, const string&);

template<typename U>
void parse_body(U& object, const string& raw, size_t body_start) {
    string te = object.headers.at("Transfer-Encoding");
    string cl = object.headers.count("Content-Length") ? request.headers.at("Content-Length") : "";

    // --- RFC 7231: GET and HEAD should not have body ---
    if (request.method == "GET" || request.method == "HEAD") {
        return;
    }

    // --- Case 1: Chunked ---
    if (!te.empty() && te.find("chunked") != string::npos) {
        size_t pos = body_start;
        string body;

        while (true) {
            size_t line_end = raw.find("\r\n", pos);
            if (line_end == string::npos)
                return {req, NEED_MORE_DATA, ""}; // incomplete chunk size

            int chunk_size = stoi(raw.substr(pos, line_end - pos), nullptr, 16);
            pos = line_end + 2;

            if (chunk_size == 0) {
                if (raw.size() < pos + 2)
                    return {req, NEED_MORE_DATA, ""}; // incomplete terminating CRLF
                pos += 2; // skip last CRLF
                req.body = body;
                string remaining = raw.substr(pos);
                return {req, COMPLETE, remaining};
            }

            if (raw.size() < pos + chunk_size + 2)
                return {req, NEED_MORE_DATA, ""}; // incomplete chunk data

            body += raw.substr(pos, chunk_size);
            pos += chunk_size + 2; // skip chunk + CRLF
        }
    }
    // --- Case 2: Content-Length ---
    else if (!cl.empty()) {
        int len = stoi(cl);
        if (raw.size() < body_start + len)
            return {req, NEED_MORE_DATA, ""}; // incomplete body

        req.body = raw.substr(body_start, len);
        string remaining = raw.substr(body_start + len);
        return {req, COMPLETE, remaining};
    }
    // --- Case 3: Neither present ---
    else {
        req.body = "";
        string remaining = raw.substr(body_start);
        return {req, COMPLETE, remaining};
    }
}

template void parse_body<HTTPRequest>(HTTPRequest&, const string&, size_t);
template void parse_body<HTTPResponse>(HTTPResponse&, const string&, size_t);
