#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <string>
#include <netinet/in.h> // sockaddr_in

class UdpClient {
private:
    int sock;
    sockaddr_in serverAddr;

public:
    UdpClient(const std::string& ip, uint16_t port);
    void sendMessage(const std::string& message);
    ~UdpClient();
};

#endif // UDPCLIENT_H
