#include <iostream>
#include <cstring>
#include <unistd.h>    // close()
#include <arpa/inet.h> // sockaddr_in, inet_pton()
#include "client.h"

UdpClient::UdpClient(const std::string &ip, uint16_t port)
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        throw std::runtime_error("Failed to create socket");
    }

    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        close(sock);
        throw std::invalid_argument("Invalid IP address");
    }
}

void UdpClient::sendMessage(const std::string &message)
{
    ssize_t sent = sendto(sock, message.c_str(), message.length(), 0,
                          (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (sent < 0)
    {
        perror("Send failed");
        throw std::runtime_error("Failed to send message");
    }
}

UdpClient::~UdpClient()
{
    close(sock);
}