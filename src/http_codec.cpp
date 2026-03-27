#include "http_codec.hpp"
#include <cstddef>
#include <cstring>
#include <sstream>

/*
* WARNING:
* The decoder function assumes the raw request/response is in standard HTTP format
* If the raw request/response is not in the standard format due to no implemention
* of error handling and consideration on the edge cases the function can
* malperform and can be unsafe.
*
* Maybe I will fix this in upcoming version.
* Till then let me go void.
*/
HTTPRequest decode_http_request(const char* raw_request) {
    HTTPRequest request;

    string raw_request_string = string(raw_request);
    request.raw = raw_request_string;

    size_t first_crlf_pos = raw_request_string.find("\r\n");
    size_t last_crlf_pos = raw_request_string.rfind("\r\n");

    request.body = raw_request_string.substr(last_crlf_pos + 2);

    string request_line = raw_request_string.substr(0, first_crlf_pos);
    istringstream iss(request_line);
    iss >> request.method >> request.path >> request.version;

    string headers = raw_request_string.substr(first_crlf_pos + 2, last_crlf_pos - (first_crlf_pos + 2));
    while (!headers.empty()) {
		string header = headers.substr(0, headers.find("\r\n"));
		size_t colon_pos = header.find(":");
		size_t value_pos = (header[colon_pos + 1] == ' ')? colon_pos + 2 : colon_pos + 1;
		request.headers[header.substr(0, colon_pos)] = header.substr(value_pos); 
		headers = headers.substr(headers.find("\r\n") + 2);
    }

    return request;
}

HTTPResponse decode_http_response(const char* raw_response) {
    HTTPResponse response;

    string raw_response_string = string(raw_response);
    response.raw = raw_response_string;

    size_t first_crlf_pos = raw_response_string.find("\r\n");
    size_t last_crlf_pos = raw_response_string.rfind("\r\n");

    response.body = raw_response_string.substr(last_crlf_pos + 2);

    string response_line = raw_response_string.substr(0, first_crlf_pos);
    istringstream iss(response_line);
    iss >> response.version >> response.status_code >> response.reason_phrase;

    string headers = raw_response_string.substr(first_crlf_pos + 2, last_crlf_pos - (first_crlf_pos + 2));
    while (!headers.empty()) {
		string header = headers.substr(0, headers.find("\r\n"));
		size_t colon_pos = header.find(":");
		size_t value_pos = (header[colon_pos + 1] == ' ')? colon_pos + 2 : colon_pos + 1;
		response.headers[header.substr(0, colon_pos)] = header.substr(value_pos); 
		headers = headers.substr(headers.find("\r\n") + 2);
    }

    return response;
}

string encode_http_request(const HTTPRequest &request) {
    if (request.method.empty() || request.path.empty() || request.version.empty()) {
        // cannot form valid request
        return "";   // or throw exception / return error code
    }

    string raw_request = request.method + " " + request.path + " " + request.version + "\r\n";

    for (const auto &header : request.headers) {
        raw_request += header.first + ": " + header.second + "\r\n";
    }

    raw_request += "\r\n";
    raw_request += request.body;

    return raw_request;
}

string encode_http_response(const HTTPResponse &response) {
    // WARNING: not handled for request.status_code
    if (response.version.empty()) {
        // cannot form valid request
        return "";   // or throw exception / return error code
    }

    string raw_response = response.version + " " + to_string(response.status_code) + " " + response.reason_phrase + "\r\n";

    for (const auto &header : response.headers) {
        raw_response += header.first + ": " + header.second + "\r\n";
    }

    raw_response += "\r\n";
    raw_response += response.body;

    return raw_response;
}
