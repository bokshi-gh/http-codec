#include "codec_helpers.hpp"

#include <sstream>
#include <stdexcept>

using namespace std;

void parse_request_target(HTTPRequest& req) {
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

    // Get headers (may be empty if not present)
    string te = object.headers.at("Transfer-Encoding");
    string cl = object.headers.count("Content-Length") 
                ? object.headers.at("Content-Length") 
                : "";

    // --- Case 1: Transfer-Encoding: chunked ---
    if (!te.empty() && te.find("chunked") != string::npos) {

        // Start reading from beginning of body
        size_t pos = body_start;

        while (true) {

            // Find end of chunk-size line (ends with \r\n)
            size_t line_end = raw.find("\r\n", pos);
            if (line_end == string::npos)
                throw invalid_argument("incomplete chunk size");

            // Extract chunk size (hex)
            int chunk_size = stoi(raw.substr(pos, line_end - pos), nullptr, 16);

            // Move position to start of chunk data
            pos = line_end + 2;

            // If chunk size is 0 → end of body
            if (chunk_size == 0) {

                // Ensure final \r\n exists
                if (raw.size() < pos + 2)
                    throw invalid_argument("incomplete terminating CRLF");

                // Skip final \r\n
                pos += 2;

                // Body parsing complete
                return;
            }

            // Check if full chunk data is available
            // +2 is for trailing \r\n after chunk data
            if (raw.size() < pos + chunk_size + 2)
                throw invalid_argument("incomplete chunk data");

            // Append chunk data to body
            object.body += raw.substr(pos, chunk_size);

            // Move position past data and trailing \r\n
            pos += chunk_size + 2;
        }
    }

    // --- Case 2: Content-Length ---
    else if (!cl.empty()) {

        // Convert length from string to integer
        int len = stoi(cl);

        // Check if we have enough data
        if (raw.size() < body_start + len)
            throw invalid_argument(
                "Content-Length mismatch: expected " + to_string(len) +
                " bytes, received " + to_string(raw.size() - body_start)
            );

        // Extract exactly Content-Length bytes
        object.body = raw.substr(body_start, len);
    }

    // --- Case 3: No body ---
    else {
        object.body = "";
    }
}

template void parse_body<HTTPRequest>(HTTPRequest&, const string&, size_t);
template void parse_body<HTTPResponse>(HTTPResponse&, const string&, size_t);

template<typename V>
string encode_body(const V &message) {
    string te = message.headers.count("Transfer-Encoding") 
                ? message.headers.at("Transfer-Encoding") 
                : "";

    string cl = message.headers.count("Content-Length") 
                ? message.headers.at("Content-Length") 
                : "";

    const string &body = message.body;

    // --- Case 1: Transfer-Encoding: chunked ---
    if (!te.empty() && te.find("chunked") != string::npos) {
        string raw;
        size_t pos = 0;
        size_t chunk_size = 4 * 1024; // 4KB chunks (adjustable)

        while (pos < body.size()) {
            size_t len = min(chunk_size, body.size() - pos);

            stringstream ss;
            ss << hex << len;
            raw += ss.str() + "\r\n";

            raw += body.substr(pos, len) + "\r\n";
            pos += len;
        }

        // final zero-length chunk
        raw += "0\r\n\r\n";
        return raw;
    }

    // --- Case 2: Content-Length ---
    else if (!cl.empty()) {
        int len = stoi(cl);
        if (len < 0)
            throw invalid_argument("Negative Content-Length");

        if (body.size() < (size_t)len)
            throw invalid_argument("Body smaller than Content-Length");

        return body.substr(0, len);
    }

    // --- Case 3: Neither present ---
    else {
        if (!body.empty())
            throw invalid_argument("Body present but no Content-Length or Transfer-Encoding");
        return "";
    }
}

template string encode_body<HTTPRequest>(const HTTPRequest&);
template string encode_body<HTTPResponse>(const HTTPResponse&);
