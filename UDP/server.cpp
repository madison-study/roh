#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.h"

UdpServer::UdpServer(uint16_t port)
{
    // 소켓 생성
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        throw std::runtime_error("Failed to create socket");
    }
    last_seq = UINT32_MAX;
    // 서버 주소 초기화
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 모든 IP로부터 수신 허용
    serverAddr.sin_port = htons(port);

    // 바인드
    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(sock);
        throw std::runtime_error("Failed to bind socket");
    }

    std::cout << "UdpServer started on port " << port << std::endl;
}

void UdpServer::receiveMessage()
{
    char buffer[BUFFER_SIZE];
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    std::memset(buffer, 0, BUFFER_SIZE);
    ssize_t received = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                (sockaddr *)&clientAddr, &clientLen);
    if (received < 0)
    {
        perror("recvfrom failed");
        return;
    }

    buffer[received] = '\0'; // null-terminate the string

    char *token = strtok(buffer, ":");
    if (token == nullptr)
        return;

    uint32_t seq = atoi(token);

    char *payload = strtok(nullptr, "");
    if (last_seq == UINT32_MAX)
    {
        last_seq = seq; // 처음 받은 패킷 번호 저장
    }
    else
    {
        if (seq > last_seq + 1)
        {
            std::cout << "Packet loss detected! Lost "
                      << (seq - last_seq - 1) << " packets.\n";
        }
        last_seq = seq;
    }

    std::cout << "Received from "
              << inet_ntoa(clientAddr.sin_addr)
              << ":" << ntohs(clientAddr.sin_port)
              << " -> " << buffer << std::endl;
}

UdpServer::~UdpServer()
{
    close(sock);
}