#include <iostream>
#include "../include/http_codec.hpp"

using namespace std;

int main() {
    // ------------------------
    // Raw HTTP Request
    // ------------------------
    const char* raw_request =
        "POST /api/v1/users?active=true&limit=10 HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/json\r\n"
        "User-Agent: custom-client\r\n"
        "\r\n"
        "{ \"name\": \"Rajesh Thapa\" }";


    // ------------------------
    // Decode Request
    // ------------------------
    HTTPRequest req = decode_http_request(raw_request);

    cout << "===== HTTP REQUEST =====\n";
    cout << "Method          : " << req.method << "\n";
    cout << "Request Target  : " << req.request_target << "\n";
    cout << "Path            : " << req.path << "\n";
    cout << "Version         : " << req.version << "\n\n";


    // ------------------------
    // Headers
    // ------------------------
    cout << "===== HEADERS =====\n";
    for (const auto& [k, v] : req.headers) {
        cout << k << ": " << v << "\n";
    }
    cout << "\n";


    // ------------------------
    // Query Parameters
    // ------------------------
    cout << "===== QUERY PARAMETERS =====\n";
    for (const auto& [k, v] : req.query_parameters) {
        cout << k << " = " << v << "\n";
    }
    cout << "\n";


    // ------------------------
    // Body
    // ------------------------
    cout << "===== BODY =====\n";
    cout << req.body << "\n\n";


    // ------------------------
    // Build HTTP Response
    // ------------------------
    HTTPResponse res;
    res.version = "HTTP/1.1";
    res.status_code = 200;
    res.reason_phrase = "OK";

    res.headers["Content-Type"] = "application/json";
    res.headers["Server"] = "my-http-lib";

    res.body = "{ \"status\": \"success\" }";


    // ------------------------
    // Encode Response
    // ------------------------
    string raw_response = encode_http_response(res);

    cout << "===== HTTP RESPONSE =====\n";
    cout << raw_response << "\n";


    // ------------------------
    // Decode Response (round-trip demo)
    // ------------------------
    HTTPResponse decoded_res = decode_http_response(raw_response.c_str());

    cout << "===== DECODED RESPONSE =====\n";
    cout << "Version : " << decoded_res.version << "\n";
    cout << "Status  : " << decoded_res.status_code << "\n";
    cout << "Reason  : " << decoded_res.reason_phrase << "\n\n";

    cout << "Headers:\n";
    for (const auto& [k, v] : decoded_res.headers) {
        cout << k << ": " << v << "\n";
    }

    cout << "\nBody:\n" << decoded_res.body << "\n";

    return 0;
}
