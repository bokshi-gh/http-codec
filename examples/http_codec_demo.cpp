#include "http_codec.hpp"

#include <iostream>

using namespace std;

int main() {
    // Example raw HTTP request
    const char* raw_request = 
        "POST /api/data?user=alice&id=123 HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Length: 27\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "{\"message\":\"Hello World!\"}";

    // --- Decode HTTP Request ---
    HTTPRequest request = decode_http_request(raw_request);

    cout << "--- Decoded HTTP Request ---\n";
    cout << "Method: " << request.method << "\n";
    cout << "Request Target: " << request.request_target << "\n";
    cout << "Path: " << request.path << "\n";
    cout << "Version: " << request.version << "\n";

    cout << "Headers:\n";
    for (auto& [k,v] : request.headers)
        cout << "  " << k << ": " << v << "\n";

    cout << "Body:\n" << request.body << "\n\n";

    // --- Encode HTTP Request ---
    string encoded_request = encode_http_request(request);
    cout << "--- Re-encoded HTTP Request ---\n";
    cout << encoded_request << "\n";

    // --- Create and Encode HTTP Response ---
    HTTPResponse response;
    response.version = request.version;
    response.status_code = 200;
    response.reason_phrase = "OK";
    response.headers["Content-Type"] = "text/plain";
    response.body = "Request processed successfully.";

    string raw_response = encode_http_response(response);
    cout << "--- Encoded HTTP Response ---\n";
    cout << raw_response << "\n";

    // --- Decode HTTP Response ---
    HTTPResponse decoded_response = decode_http_response(raw_response);
    cout << "--- Decoded HTTP Response ---\n";
    cout << "Version: " << decoded_response.version << "\n";
    cout << "Status Code: " << decoded_response.status_code << "\n";
    cout << "Reason Phrase: " << decoded_response.reason_phrase << "\n";
    cout << "Headers:\n";
    for (auto& [k,v] : decoded_response.headers)
        cout << "  " << k << ": " << v << "\n";
    cout << "Body:\n" << decoded_response.body << "\n";

    return 0;
}
