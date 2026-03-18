#include "dns_parse.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int parse_name(uint8_t *buffer, int offset, char *out, int *size)
{
    int i = offset;
    int out_pos = 0;
    int jumped = 0;
    int bytes_read = 0;
    int dot_needed = 0;
    int jump_count = 0;

    while (1)
    {

        if (i >= *size)
        {
            printf("[ERROR] Offset exceeded packet size\n");
            return -1;
        }

        uint8_t len = buffer[i];

        if (len == 0)
        {
            if (!jumped)
                bytes_read = (i - offset) + 1;
            break;
        }

        /* compression pointer */
        if ((len & 0xC0) == 0xC0)
        {

            if (i + 1 >= *size)
            {
                printf("[ERROR] Pointer out of bounds\n");
                return -1;
            }

            int target_offset = ((len & 0x3F) << 8) | buffer[i + 1];

            if (!jumped)
            {
                bytes_read = (i - offset) + 2;
            }

            i = target_offset;
            jumped = 1;

            if (++jump_count > 10)
            {
                printf("[ERROR] Too many compression jumps\n");
                return -1;
            }

            continue;
        }

        /* label */
        i++;
        if (i + len > *size)
        {
            printf("[ERROR] Label exceeds packet size\n");
            return -1;
        }

        if (dot_needed)
            out[out_pos++] = '.';

        for (int j = 0; j < len; j++)
        {

            if (out_pos >= 255)
            {
                printf("[ERROR] Name buffer overflow\n");
                return -1;
            }

            out[out_pos++] = buffer[i++];
        }

        out[out_pos] = '\0';

        dot_needed = 1;
    }

    out[out_pos] = '\0';

    printf("[DEBUG] Final parsed name: %s, Next offset in packet: %d\n", out, offset + bytes_read);

    return bytes_read;
}
int parse_dns_response(uint8_t *buffer, int size,
                       dns_rr_t *answers,
                       int *answer_count,
                       nameserver_list_t *ns_list)
{
    printf("RR struct size: %lu\n", sizeof(dns_rr_fixed_t));
    dns_header_t *header = (dns_header_t *)buffer;

    int qdcount = ntohs(header->qdcount);
    int ancount = ntohs(header->ancount);
    int nscount = ntohs(header->nscount);
    int arcount = ntohs(header->arcount);

    int offset = sizeof(dns_header_t);

    char name_buf[256];

    printf("\n--- [Section Parsing Start] ---\n");
    printf("[DEBUG] size: %d\n", size);

    /* QUESTION */

    for (int i = 0; i < qdcount; i++)
    {

        int read = parse_name(buffer, offset, name_buf, &size);
        if (read < 0)
            return -1;

        offset += read;

        if (offset + 4 > size)
            return -1;

        offset += 4;

        printf("[Q] Question for: %s\n", name_buf);
    }

    /* ANSWER */

    *answer_count = 0;

    for (int i = 0; i < ancount; i++)
    {

        int read = parse_name(buffer, offset, name_buf, &size);
        if (read < 0)
            return -1;

        offset += read;

        if (offset + sizeof(dns_rr_fixed_t) > size)
            return -1;

        dns_rr_fixed_t *rr = (dns_rr_fixed_t *)(buffer + offset);

        uint16_t type = ntohs(rr->type);
        uint16_t rdlen = ntohs(rr->rdlength);

        offset += sizeof(dns_rr_fixed_t);

        if (offset + rdlen > size)
            return -1;

        if (type == DNS_TYPE_A && *answer_count < MAX_ANSWERS)
        {

            uint8_t *ip = buffer + offset;

            printf("[AN] Final Answer IP: %d.%d.%d.%d\n",
                   ip[0], ip[1], ip[2], ip[3]);

            memcpy(answers[*answer_count].rdata, ip, 4);

            (*answer_count)++;
        }

        offset += rdlen;
    }

    /* AUTHORITY */

    ns_list->count = 0;

    for (int i = 0; i < nscount; i++)
    {

        int read = parse_name(buffer, offset, name_buf, &size);
        if (read < 0)
            return -1;

        offset += read;

        if (offset + sizeof(dns_rr_fixed_t) > size)
        {
            printf("[ERROR] RR header overflow\n");
            return -1;
        }

        dns_rr_fixed_t *rr = (dns_rr_fixed_t *)(buffer + offset);

        uint16_t type = ntohs(rr->type);
        uint16_t rdlen = ntohs(rr->rdlength);

        offset += sizeof(dns_rr_fixed_t);

        if (offset + rdlen > size)
        {
            printf("[ERROR] RDATA overflow\n");
            return -1;
        }

        if (type == DNS_TYPE_NS && ns_list->count < MAX_NAMESERVERS)
        {

            int tmp = offset;

            if (parse_name(buffer, tmp,
                           ns_list->servers[ns_list->count].name,
                           &size) < 0)
                return -1;

            memset(ns_list->servers[ns_list->count].ip, 0, MAX_NS_IP);

            printf("[NS] Authority Server Found: %s\n",
                   ns_list->servers[ns_list->count].name);

            ns_list->count++;
        }

        offset += rdlen;
    }
    printf("check point\n");

    /* ADDITIONAL */

    for (int i = 0; i < arcount; i++)
    {

        char ar_name[256];

        int read = parse_name(buffer, offset, ar_name, &size);
        if (read < 0)
            return -1;

        offset += read;

        if (offset + sizeof(dns_rr_fixed_t) > size)
            return -1;

        dns_rr_fixed_t *rr = (dns_rr_fixed_t *)(buffer + offset);

        uint16_t type = ntohs(rr->type);
        uint16_t rdlen = ntohs(rr->rdlength);

        offset += sizeof(dns_rr_fixed_t);

        if (offset + rdlen > size)
            return -1;

        if (type == DNS_TYPE_A)
        {

            for (int j = 0; j < ns_list->count; j++)
            {

                if (strcmp(ns_list->servers[j].name, ar_name) == 0)
                {

                    uint8_t *ip = buffer + offset;

                    sprintf(ns_list->servers[j].ip,
                            "%d.%d.%d.%d",
                            ip[0], ip[1], ip[2], ip[3]);

                    printf("[AR] Match! Glue Record for %s: %s\n",
                           ar_name,
                           ns_list->servers[j].ip);

                    break;
                }
            }
        }

        offset += rdlen;
    }

    printf("--- [Section Parsing End] ---\n");

    return 0;
}