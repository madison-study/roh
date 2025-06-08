#pragma once

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

class TLSClient {
private:
    int sock = -1;
    SSL_CTX* ctx = nullptr;
    SSL* ssl = nullptr;

    void createSocketAndConnect(const std::string& ip, uint16_t port);

public:
    TLSClient(const std::string& ip, uint16_t port);
    ~TLSClient();

    void sendMessage(const std::string& message);
    std::string recvMessage(size_t bufSize = 1024);
};
