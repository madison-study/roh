#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include <netinet/in.h> // sockaddr_in

class TCPClient {
private:
    int sock;
    sockaddr_in serverAddr;

public:
    TCPClient(const std::string& ip, uint16_t port);
    void sendMessage(const std::string& message);
    ~TCPClient();
};

#endif // TCPCLIENT_H
