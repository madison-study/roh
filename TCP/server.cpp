#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
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
    std::cout << "TCPServer started on port " << port << std::endl;
}

void TCPServer::handleClient(int clientSock, sockaddr_in clientAddr) {
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t received = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (received <= 0) {
            if (received == 0)
                std::cout << "Client disconnected\n";
            else
                perror("recv failed");
            break;
        }
        buffer[received] = '\0';

        // 메시지 처리 (예: 패킷 번호, payload 분리)

        // 패킷 손실 체크
        // if (last_seq == UINT32_MAX) {
        //     last_seq = seq;
        // } else if (seq > last_seq + 1) {
        //     std::cout << "Packet loss detected! Lost " << (seq - last_seq - 1) << " packets.\n";
        //     last_seq = seq;
        // } else {
        //     last_seq = seq;
        // }

        std::cout << "Received from "
                  << inet_ntoa(clientAddr.sin_addr) << ":"
                  << ntohs(clientAddr.sin_port)
                  << " -> " << buffer << "\n";
    }
    close(clientSock);
}

void TCPServer::receiveMessage() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSock = accept(sock, (sockaddr *)&clientAddr, &clientLen);
    if (clientSock < 0) {
        perror("Accept failed");
        return;
    } else {
        std::cout<< "Connected" << std::endl;
    }
    // 클라이언트 하나에 대해 메시지 처리 시작
    handleClient(clientSock, clientAddr);
}

TCPServer::~TCPServer()
{
    close(sock);
}