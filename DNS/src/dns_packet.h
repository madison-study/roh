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
/* DNS Resource Record (Parsed) */
/* ===================== */

typedef struct
{
    char name[256];     // 도메인 이름 (예: google.com)
    uint16_t type;      // 레코드 타입 (A, NS 등)
    uint16_t class;     // 클래스 (보통 IN)
    uint32_t ttl;       // 수명 (Time to Live)
    uint16_t rdlength;  // 데이터 길이
    uint8_t rdata[256]; // 실제 데이터 (IP 주소 등)
    
} dns_rr_t;

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

typedef struct __attribute__((packed)) {
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
    const char *domain,
    uint8_t *buffer
    );


#endif