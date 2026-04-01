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

            // trim leading space after colon
            size_t value_start = colon + 1;
            while (value_start < header.size() && header[value_start] == ' ')
                value_start++;

            string value = header.substr(value_start);

            object.headers[key].push_back(value);
        }

        pos = end + 2;
    }
}

template void parse_headers<HTTPRequest>(HTTPRequest&, const string&);
template void parse_headers<HTTPResponse>(HTTPResponse&, const string&);
