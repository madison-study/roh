
#include "dns_packet.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

int dns_encode_domain(const char *domain, uint8_t *buffer)
{
    int src_idx = 0; // Index for the original domain string
    int dst_idx = 0; // Index for the destination buffer
    int len_idx = 0; // Position where the length of the current label is stored
    int count = 0;   // Character count of the current label

    // Start filling the buffer from index 1,
    // leaving index 0 for the first label's length.
    dst_idx = 1;

    while (domain[src_idx] != '\0')
    {
        if (domain[src_idx] == '.')
        {
            // When a dot is met, write the length of the label just finished
            buffer[len_idx] = (uint8_t)count;

            // Mark the current position as the spot for the next label's length
            len_idx = dst_idx;
            dst_idx++;

            // Reset character count for the next label
            count = 0;
        }
        else
        {
            // Copy character from domain string to buffer
            buffer[dst_idx] = (uint8_t)domain[src_idx];
            dst_idx++;
            count++;
        }
        src_idx++;
    }

    // Write the length of the last label (the part after the last dot)
    buffer[len_idx] = (uint8_t)count;

    // DNS name field must end with a null byte (zero-length label)
    buffer[dst_idx] = 0x00;
    dst_idx++;

    // Return the total size of the encoded domain name in bytes
    return dst_idx;
}

int dns_decode_name(const uint8_t *packet, int packet_len, int offset, char *out, int out_len)
{
    int i = offset;
    int out_pos = 0;
    int jumped = 0;
    int jumped_offset = 0;
    int bytes_read = 0;
    printf("RR struct size: %lu\n", sizeof(dns_rr_fixed_t));
    while (packet[i] != 0)
    {
        // 압축 포인터 체크 (상위 2비트가 11인 경우)
        if ((packet[i] & 0xC0) == 0xC0)
        {
            if (!jumped)
            {
                bytes_read = (i - offset) + 2;
            }
            // 포인터 값 계산 (14비트)
            i = ((packet[i] & 0x3F) << 8) | packet[i + 1];
            jumped = 1;
        }
        else
        {
            int len = packet[i++];
            if (out_pos + len + 1 > out_len)
                return -1; // 버퍼 초과

            if (out_pos > 0)
                out[out_pos++] = '.';
            memcpy(&out[out_pos], &packet[i], len);
            out_pos += len;
            i += len;
        }
    }
    out[out_pos] = '\0';

    if (!jumped)
        bytes_read = i - offset + 1;
    return bytes_read; // 패킷 내에서 소모한 바이트 수 반환
}

int dns_build_query(uint8_t *buffer, const char *domain, uint16_t qtype)
{
    dns_header_t *header = (dns_header_t *)buffer;

    // 1. 헤더 설정
    header->id = htons(0x1234);    // 임의의 ID
    header->flags = htons(0x0100); // Standard Query (Recursion Desired)
    header->qdcount = htons(1);    // 질문 1개
    header->ancount = 0;
    header->nscount = 0;
    header->arcount = 0;

    // 2. 이름 인코딩 (헤더 12바이트 이후 위치)
    int name_len = dns_encode_domain(domain, buffer + sizeof(dns_header_t));

    // 3. 질문 정보 추가 (Type, Class)
    dns_question_t *q = (dns_question_t *)(buffer + sizeof(dns_header_t) + name_len);
    q->qtype = htons(qtype);
    q->qclass = htons(DNS_CLASS_IN);

    return sizeof(dns_header_t) + name_len + sizeof(dns_question_t);
}

int dns_parse_response(uint8_t *packet, int packet_len)
{
    dns_header_t *header = (dns_header_t *)packet;

    // 네트워크 바이트 오더를 호스트 바이트 오더로 변환
    uint16_t qdcount = ntohs(header->qdcount);
    uint16_t ancount = ntohs(header->ancount);
    uint16_t flags = ntohs(header->flags);

    // 에러 체크 (RCODE 추출: 하위 4비트)
    if ((flags & 0x000F) != 0)
    {
        printf("DNS Error: RCODE %d\n", flags & 0x000F);
        return -1;
    }

    int offset = sizeof(dns_header_t);
    char name[256];

    // 1. 질문 섹션 처리 (단순히 건너뜀)
    for (int i = 0; i < qdcount; i++)
    {
        int read = dns_decode_name(packet, packet_len, offset, name, sizeof(name));
        offset += read; // 이름 길이만큼 이동
        offset += 4;    // qtype(2) + qclass(2) 건너뜀
    }

    // 2. 답변 섹션 처리
    printf("--- Answers (%d) ---\n", ancount);
    for (int i = 0; i < ancount; i++)
    {
        // 도메인 이름 해독
        int read = dns_decode_name(packet, packet_len, offset, name, sizeof(name));
        offset += read;

        // 고정 필드(Type, Class, TTL, RDLength) 읽기
        dns_rr_fixed_t *rr = (dns_rr_fixed_t *)(packet + offset);
        uint16_t type = ntohs(rr->type);
        uint16_t rdlen = ntohs(rr->rdlength);
        offset += sizeof(dns_rr_fixed_t);

        // A 레코드(IP 주소)인 경우 출력
        if (type == DNS_TYPE_A && rdlen == 4)
        {
            uint8_t *ip = packet + offset;
            printf("Name: %s -> IP: %d.%d.%d.%d\n", name, ip[0], ip[1], ip[2], ip[3]);
        }

        offset += rdlen; // 다음 레코드로 이동
    }

    return 0;
}