#ifndef DNS_ENCODE_H
#define DNS_ENCODE_H

#include <stdint.h>

int build_dns_query(
    const char *domain,
    uint8_t *buffer
);

int encode_domain(
    const char *domain,
    uint8_t *buffer
);

#endif