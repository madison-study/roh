#include "udp_client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DNS_PORT 53
#define TIMEOUT_SEC 3

int udp_send_query(
    const char *server_ip,
    uint8_t *query,
    int query_len,
    uint8_t *response,
    int *response_len)
{
    int sock;                                 // file descriptor that points created socket
    struct sockaddr_in server_addr;           // server address
    socklen_t addr_len = sizeof(server_addr); // store address struct's size

    sock = socket(AF_INET, SOCK_DGRAM, 0); // create socket with AF_INET: IPv4 SOCK_DGRAM: UDP
    if (sock < 0)
    {
        perror("socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));   // init struct
    server_addr.sin_family = AF_INET;               // IPv4 
    server_addr.sin_port = htons(DNS_PORT);         // set port num

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sock);
        return -1;
    }

    int sent = sendto(  // send data, on success return sent bytes, on failure -1
        sock,
        query,
        query_len,
        0,
        (struct sockaddr *)&server_addr,
        addr_len
    );

    if (sent < 0)
    {
        perror("sendto");
        close(sock);
        return -1;
    }

    int received = recvfrom( // blocking,, waits for response
        sock,
        response,
        *response_len,
        0,
        NULL,
        NULL);

    if (received < 0)
    {
        perror("recvfrom");
        close(sock);
        return -1;
    }

    *response_len = received;
    close(sock);

    return 0;
}