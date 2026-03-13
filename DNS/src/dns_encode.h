#ifndef DNS_ENCODE_H
#define DNS_ENCODE_H

#include <stdint.h>

int encode_domain(
    const char *domain,
    uint8_t *buffer
);

#endif