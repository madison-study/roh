#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <netinet/in.h> // sockaddr_in

class TCPServer
{
private:
    int sock;
    sockaddr_in serverAddr;
    static const int BUFFER_SIZE = 4024;
    uint32_t last_seq;
    void handleClient(int clientSock, sockaddr_in clientAddr);
public:
    TCPServer(uint16_t port);
    void receiveMessage();
    ~TCPServer();
};

#endif // TCPSERVER_H
