#include "TLSClient.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

void TLSClient::createSocketAndConnect(const std::string& ip, uint16_t port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        close(sock);
        throw std::invalid_argument("Invalid IP address");
    }

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        close(sock);
        throw std::runtime_error("Failed to connect");
    }
}

TLSClient::TLSClient(const std::string& ip, uint16_t port) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) throw std::runtime_error("Failed to create SSL_CTX");

    createSocketAndConnect(ip, port);

    ssl = SSL_new(ctx);
    if (!ssl) {
        SSL_CTX_free(ctx);
        close(sock);
        throw std::runtime_error("Failed to create SSL structure");
    }

    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        throw std::runtime_error("Failed SSL handshake");
    }
}

TLSClient::~TLSClient() {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    if (ctx) SSL_CTX_free(ctx);
    if (sock >= 0) close(sock);
}

void TLSClient::sendMessage(const std::string& message) {
    size_t totalSent = 0;
    size_t len = message.size();

    while (totalSent < len) {
        int sent = SSL_write(ssl, message.data() + totalSent, (int)(len - totalSent));
        if (sent <= 0) {
            throw std::runtime_error("SSL_write failed");
        }
        totalSent += sent;
    }
}

std::string TLSClient::recvMessage(size_t bufSize) {
    char buffer[bufSize + 1];
    int received = SSL_read(ssl, buffer, (int)bufSize);
    if (received <= 0) {
        throw std::runtime_error("SSL_read failed or connection closed");
    }
    buffer[received] = '\0';
    return std::string(buffer);
}
