#pragma once

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <resolv.h>

class TLSClient {
private:
    u_char response[NS_PACKETSZ]; // u_char depends on compiler it can be signed or unsigned
    int sock = -1;
    SSL_CTX* ctx = nullptr;
    SSL* ssl = nullptr;

    void createSocketAndConnect(const std::string& ip, uint16_t port);
    std::string get_best_mx_record(const std::string& domain);
public:
    TLSClient(const std::string& domain, uint16_t port);
    ~TLSClient();

    void sendMessage(const std::string& message);
    std::string recvMessage(size_t bufSize = 1024);
};
