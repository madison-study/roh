#include "udp_client.h"
#include "dns_packet.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define DNS_PORT 53
#define TIMEOUT_SEC 3

int udp_send_query(
    const char *server_ip,
    uint8_t *query,
    int query_len,
    uint8_t *response,
    int *response_len)
{
    int sock;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    // 1. 소켓 생성 확인
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("  [Socket Error]");
        return -1;
    }
    // printf("  [Debug] UDP Socket created (fd: %d)\n", sock);

    // 2. 타임아웃 설정 (응답 없을 시 무한 대기 방지)
    struct timeval tv;
    tv.tv_sec = 2; // 2초 타임아웃
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("  [Socket Error] setsockopt failed");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(53); // DNS_PORT

    // 3. IP 주소 변환 확인
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "  [Debug] Invalid IP address: %s\n", server_ip);
        close(sock);
        return -1;
    }

    // 4. 데이터 전송 시도
    // printf("  [Debug] Sending %d bytes to %s...\n", query_len, server_ip);
    int sent = sendto(
        sock,
        query,
        query_len,
        0,
        (struct sockaddr *)&server_addr,
        addr_len);

    if (sent < 0)
    {
        perror("  [Socket Error] sendto failed");
        close(sock);
        return -1;
    }
    // printf("  [Debug] Successfully sent %d bytes.\n", sent);

    // // 5. 데이터 수신 대기
    // printf("  [Debug] Waiting for response (timeout: 2s)...\n");

    // 중요: *response_len 대신 실제 버퍼 최대 크기(예: 512)를 넣어야 합니다.
    int received = recvfrom(
        sock,
        response,
        DNS_MAX_PACKET_SIZE, // 이 값이 0이면 절대 못 받습니다.
        0,
        NULL,
        NULL);

    if (received < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            fprintf(stderr, "  [Debug] Receive Timeout! No response from server.\n");
        }
        else
        {
            perror("  [Socket Error] recvfrom failed");
        }
        close(sock);
        return -1;
    }

    // printf("  [Debug] Received %d bytes from server.\n", received);
    // printf("  [DEBUG] original res len: %d\n", *response_len);
    *response_len = received;
    // printf("  [DEBUG] after res len: %d\n", *response_len);
    close(sock);
    // printf("  [DEBUG] prepare ret\n");
    return 0;
}