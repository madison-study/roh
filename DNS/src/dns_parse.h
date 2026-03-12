#ifndef DNS_PARSE_H
#define DNS_PARSE_H

#include <stdint.h>
#include "dns_packet.h"
#include "nameserver.h"

int parse_dns_response(
    uint8_t *buffer,
    int size,
    dns_rr_t *answers,
    int *answer_count,
    nameserver_list_t *ns_list
);

int parse_name(
    uint8_t *buffer,
    int offset,
    char *out
);

#endif