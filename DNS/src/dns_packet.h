#ifndef DNS_PACKET_H
#define DNS_PACKET_H

#include <stdint.h>

#define DNS_MAX_PACKET_SIZE 512

/* DNS Record Types */
#define DNS_TYPE_A     1
#define DNS_TYPE_NS    2
#define DNS_TYPE_CNAME 5
#define DNS_TYPE_SOA   6
#define DNS_TYPE_PTR   12
#define DNS_TYPE_MX    15
#define DNS_TYPE_TXT   16
#define DNS_TYPE_AAAA  28

/* DNS Class */
#define DNS_CLASS_IN 1


/* ===================== */
/* DNS Header (12 bytes) */
/* ===================== */

typedef struct
{
    uint16_t id;

    uint16_t flags;

    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;

} dns_header_t;


/* ===================== */
/* DNS Question */
/* ===================== */

typedef struct
{
    uint16_t qtype;
    uint16_t qclass;

} dns_question_t;


/* ===================== */
/* DNS Resource Record */
/* ===================== */

typedef struct
{
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;

} dns_rr_fixed_t;


/* ===================== */
/* Packet Builder */
/* ===================== */

int dns_build_query(
    uint8_t *buffer,
    const char *domain,
    uint16_t qtype);


/* ===================== */
/* Name Encoding */
/* ===================== */

int dns_encode_domain(
    uint8_t *buffer,
    const char *domain);


/* ===================== */
/* Name Decoding */
/* ===================== */

int dns_decode_name(
    const uint8_t *packet,
    int packet_len,
    int offset,
    char *out,
    int out_len);


/* ===================== */
/* Response Parser */
/* ===================== */

int dns_parse_response(
    uint8_t *packet,
    int packet_len);


#endif