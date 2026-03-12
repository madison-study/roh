#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <stdint.h>

int udp_send_query(
    const char *server_ip,
    uint8_t *query,
    int query_len,
    uint8_t *response,
    int *response_len
);

#endif