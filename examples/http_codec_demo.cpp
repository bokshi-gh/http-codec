#include "http_codec.hpp"

#include <iostream>

using namespace std;

int main() {
    // -----------------------------
    // Example: raw HTTP request
    // -----------------------------
    const char* raw_request =
        "GET /hello?name=ChatGPT HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: MyTestClient/1.0\r\n"
        "Accept: text/html,application/json\r\n"
        "Set-Cookie: session=abc123\r\n"
        "Set-Cookie: theme=dark\r\n"
        "\r\n"
        "body-content-goes-here";

    // Decode request
    HTTPRequest request = decode_http_request(raw_request);

    cout << "=== Decoded HTTP Request ===" << endl;
    cout << "Method: " << request.method << endl;
    cout << "Request Target: " << request.request_target << endl;
    cout << "Version: " << request.version << endl;
    cout << "Headers:" << endl;
    for (const auto &h : request.headers) {
        cout << "  " << h.first << ": ";
        for (const auto &v : h.second) cout << v << " | ";
        cout << endl;
    }
    cout << "Body: " << request.body << endl;

    // Encode back to raw HTTP
    string encoded_request = encode_http_request(request);
    cout << "\n=== Encoded HTTP Request ===" << endl;
    cout << encoded_request << endl;

    // -----------------------------
    // Example: raw HTTP response
    // -----------------------------
    const char* raw_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Set-Cookie: user=123\r\n"
        "Set-Cookie: theme=light\r\n"
        "\r\n"
        "Hello, world!";

    // Decode response
    HTTPResponse response = decode_http_response(raw_response);

    cout << "\n=== Decoded HTTP Response ===" << endl;
    cout << "Version: " << response.version << endl;
    cout << "Status Code: " << response.status_code << endl;
    cout << "Reason Phrase: " << response.reason_phrase << endl;
    cout << "Headers:" << endl;
    for (const auto &h : response.headers) {
        cout << "  " << h.first << ": ";
        for (const auto &v : h.second) cout << v << " | ";
        cout << endl;
    }
    cout << "Body: " << response.body << endl;

    return 0;
}
