#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <netinet/tcp.h>
#include "server.h"

TCPServer::TCPServer(uint16_t port)
{
    // TCP 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        throw std::runtime_error("Failed to create socket");
    }
    last_seq = UINT32_MAX;
    // 서버 주소 초기화
    memset(&serverAddr, 0, sizeof(serverAddr));
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

    if (listen(sock, 5) < 0)
    {
        perror("Listen failed");
        close(sock);
        throw std::runtime_error("Failed to listen");
    }

#ifdef __APPLE__
    int keepalive = 10;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPALIVE, &keepalive, sizeof(keepalive));
#else
    int keepidle = 30;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
#endif
    int keepintvl = 5;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));

    int keepcnt = 3;
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
    std::cout << "TCPServer started on port " << port << std::endl;
}

void TCPServer::handleClient(int clientSock, sockaddr_in clientAddr)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        ssize_t received = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (received <= 0)
        {
            if (received == 0)
                std::cout << "Client disconnected\n";
            else
                perror("recv failed");
            break;
        }
        buffer[received] = '\0';

        std::cout << "Received from "
                  << inet_ntoa(clientAddr.sin_addr) << ":"
                  << ntohs(clientAddr.sin_port)
                  << " -> " << buffer << "\n";
    }
    close(clientSock);
}

void TCPServer::handleClientPrefix(int clientSock, sockaddr_in clientAddr)
{
    std::vector<char> streamBuffer;
    uint32_t expectedLength = 0;

    while (true)
    {
        char buffer[BUFFER_SIZE];
        ssize_t received = recv(clientSock, buffer, BUFFER_SIZE, 0);
        if (received <= 0)
        {
            if (received == 0)
                std::cout << "Client disconnected\n";
            else
                perror("recv failed");
            break;
        }

        // streamBuffer에 이어붙임
        streamBuffer.insert(streamBuffer.end(), buffer, buffer + received);

        // 처리 가능한 메시지가 있는지 반복 확인
        while (true)
        {
            if (expectedLength == 0)
            {
                if (streamBuffer.size() < 4)
                    break; // 길이 프리픽스 수신 대기
                uint32_t len_net;
                memcpy(&len_net, streamBuffer.data(), 4);
                expectedLength = ntohl(len_net);
                streamBuffer.erase(streamBuffer.begin(), streamBuffer.begin() + 4);
            }

            if (streamBuffer.size() < expectedLength)
                break; // 전체 메시지 수신 대기

            std::string message(streamBuffer.begin(), streamBuffer.begin() + expectedLength);
            streamBuffer.erase(streamBuffer.begin(), streamBuffer.begin() + expectedLength);
            expectedLength = 0;

            // ✅ 여기서 message 처리
            std::cout << "Received from "
                      << inet_ntoa(clientAddr.sin_addr) << ":"
                      << ntohs(clientAddr.sin_port)
                      << " -> " << message << "\n";
        }
    }
    close(clientSock);
}

void TCPServer::receiveMessage(bool isPrefix)
{
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSock = accept(sock, (sockaddr *)&clientAddr, &clientLen);
    if (clientSock < 0)
    {
        perror("Accept failed");
        return;
    }
    else
    {
        std::cout << "Connected" << std::endl;
    }
    // 클라이언트 하나에 대해 메시지 처리 시작
    if (isPrefix)
    {
        handleClientPrefix(clientSock, clientAddr);
    }
    else
    {
        handleClient(clientSock, clientAddr);
    }
}

TCPServer::~TCPServer()
{
    close(sock);
}