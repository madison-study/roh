#include <stdio.h>
#include <string.h>
#include "resolver.h"
#include "udp_client.h"
#include "dns_packet.h"
#include "dns_parse.h"
#include "nameserver.h"

#define ROOT_SERVER_IP "198.41.0.4"

int resolve_domain(const char *domain, char *result_ip, int depth)
{
    if (depth > 5) return -1;
    uint8_t query_packet[DNS_MAX_PACKET_SIZE];
    uint8_t response_packet[DNS_MAX_PACKET_SIZE];
    int response_len = 0;

    // 현재 물어볼 서버 IP (최초에는 루트 서버)
    char current_server_ip[MAX_NS_IP];
    strcpy(current_server_ip, ROOT_SERVER_IP);

    printf("[Iterative Resolution Start] Domain: %s\n", domain);

    while (1)
    {
        printf("  -> Querying Server: %s\n", current_server_ip);

        // 1. 패킷 생성 (중요: 반복 질의이므로 RD 비트를 0으로 설정하는 것이 정석이나, 1이어도 무방)
        int query_len = dns_build_query(query_packet, domain, DNS_TYPE_A);

        // 2. UDP 통신
        printf("\n================ [DNS Request Metadata] ================\n");
        printf("Target Server IP   : %s\n", current_server_ip);
        printf("Target Port        : 53 (DNS)\n");
        printf("Protocol           : UDP\n");
        printf("Query Domain       : %s\n", domain);
        printf("Payload Size       : %d bytes\n", query_len);
        int ret = udp_send_query(current_server_ip, query_packet, query_len, response_packet, &response_len);
        // printf("[Debug] udp DONE\n");
        fflush(stdout);
        if (response_len <= 0)
        {
            printf("     Error: UDP communication failed.\n     response_len: %d\n", response_len);
            return -1;
        }

        // 3. 응답 파싱
        dns_rr_t answers[MAX_NAMESERVERS];
        int answer_count = 0;
        nameserver_list_t ns_list;
        memset(&ns_list, 0, sizeof(ns_list));

        if (parse_dns_response(response_packet, response_len, answers, &answer_count, &ns_list) != 0)
            return -1;

        // (A) 답을 찾은 경우
        if (answer_count > 0)
        {
            uint8_t *ip = (uint8_t *)answers[0].rdata;
            sprintf(result_ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
            return 0;
        }

        // (B) 다음 서버 정보(NS)가 있는 경우
        if (ns_list.count > 0)
        {
            // 1. Additional Section에 IP가 이미 파싱되어 있는지 확인 (Glue Record 존재 시)
            if (strlen(ns_list.servers[0].ip) > 0)
            {
                strcpy(current_server_ip, ns_list.servers[0].ip);
                continue;
            }
            else
            {
                // 2. Glue Record가 없는 경우 (Case A)
                // 네임서버 이름(예: ns1.awsdns.com)의 IP를 찾기 위해 자기 자신을 재귀 호출!
                printf("[Recursive] No Glue Record. Resolving NS: %s\n", ns_list.servers[0].name);

                char ns_ip[MAX_NS_IP];
                if (resolve_domain(ns_list.servers[0].name, ns_ip, depth + 1) == 0)
                {
                    // 네임서버의 IP를 알아냈으므로, 이 IP를 가지고 조회를 이어감
                    strcpy(current_server_ip, ns_ip);
                    continue;
                }
                else
                {
                    printf("Error: Could not resolve Nameserver IP for %s\n", ns_list.servers[0].name);
                    return -1;
                }
            }
        }
        break;
    }
    return -1;
}