#include <iostream>
#include <cstring>
#include <unistd.h>    // close()
#include <arpa/inet.h> // sockaddr_in, inet_pton()
#include "client.h"

TCPClient::TCPClient(const std::string &ip, uint16_t port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        throw std::runtime_error("Failed to create socket");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        close(sock);
        throw std::invalid_argument("Invalid IP address");
    }

    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Connect failed");
        close(sock);
        throw std::runtime_error("Failed to connect to server");
    }
}

void TCPClient::sendMessage(const std::string &message, const bool isPrefix)
{
    if (!isPrefix)
    {
        ssize_t sent = send(sock, message.c_str(), message.length(), 0);
        if (sent < 0)
        {
            perror("Send failed");
            throw std::runtime_error("Failed to send message");
        }
        return;
    }

    uint32_t len = htonl(message.length()); // 메시지 길이를 네트워크 바이트 순서로 변환
    std::string full_msg(reinterpret_cast<char *>(&len), sizeof(len));
    full_msg += message;

    std::cout << "[TCPClient] Sending message (" << message.length() << " bytes): " << message << std::endl;

    ssize_t total_sent = 0;
    ssize_t to_send = full_msg.length();

    while (total_sent < to_send)
    {
        ssize_t sent = send(sock, full_msg.c_str() + total_sent, to_send - total_sent, 0);
        if (sent <= 0)
        {
            perror("Send failed");
            throw std::runtime_error("Failed to send message");
        }
        total_sent += sent;
    }
}

TCPClient::~TCPClient()
{
    close(sock);
}