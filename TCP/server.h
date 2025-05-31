#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <netinet/in.h> // sockaddr_in

class TCPServer
{
private:
    int sock;
    sockaddr_in serverAddr;
    static const int BUFFER_SIZE = 1024;
    uint32_t last_seq;
    void handleClient(int clientSock, sockaddr_in clientAddr);
    void handleClientPrefix(int clientSock, sockaddr_in clientAddr);

public:
    TCPServer(uint16_t port);
    void receiveMessage(bool isPrefix);
    ~TCPServer();
};

#endif // TCPSERVER_H
