#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <string>
#include <netinet/in.h> // sockaddr_in

class UdpServer
{
private:
    int sock;
    sockaddr_in serverAddr;
    static const int BUFFER_SIZE = 1024;
    uint32_t last_seq;

public:
    UdpServer(uint16_t port);
    void receiveMessage();
    ~UdpServer();
};

#endif // UDPCLIENT_H
