#include <stdio.h>
#include <string.h>
#include "resolver.h"
#include "udp_client.h"
#include "dns_packet.h"
#include "dns_parse.h"
#include "nameserver.h"

#define ROOT_SERVER_IP "198.41.0.4"

int resolve_domain(const char *domain, char *result_ip)
{
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
        printf("[Debug] udp DONE\n");
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
        memset(&ns_list, 0, sizeof(ns_list)); // 초기화 필수

        printf("[Debug] Calling parse_dns_response (packet_len: %d)\n", response_len);
        fflush(stdout);

        printf("\n--- Packet Dump (%d bytes) ---\n", response_len);
        for (int i = 0; i < response_len; i++)
        {
            printf("%02X ", response_packet[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
            else if ((i + 1) % 8 == 0)
                printf("  ");
        }
        printf("\n------------------------------\n");

        if (parse_dns_response(response_packet, response_len, answers, &answer_count, &ns_list) != 0)
        {
            printf("[Debug] parse_dns_response returned ERROR\n");
            return -1;
        }

        printf("\n[Debug] Parse Finished. Answer: %d, NS: %d\n", answer_count, ns_list.count);
        printf("\n================ [DNS Response Analysis] ================\n");
        printf("  - Answer Count: %d\n", answer_count);
        printf("  - Authority NS Count: %d\n", ns_list.count);
        printf("========================================================\n");
        // 4. 결과 분석 로직
        fflush(stdout);
        // (A) 답을 찾은 경우 (Answer Section에 A 레코드가 있음)
        if (answer_count > 0)
        {
            uint8_t *ip = (uint8_t *)answers[0].rdata;
            sprintf(result_ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
            printf("[Success] Resolved: %s\n", result_ip);
            return 0;
        }

        // (B) 다음 서버 정보가 있는 경우 (Authority Section에 NS가 있음)
        if (ns_list.count > 0)
        {
            // 실제 로우레벨 구현에서는 Additional Section에 있는 IP를 가져와야 합니다.
            // 여기서는 단순화를 위해 첫 번째 네임서버의 IP가 ns_list에 채워졌다고 가정합니다.
            if (strlen(ns_list.servers[0].ip) > 0)
            {
                strcpy(current_server_ip, ns_list.servers[0].ip);
                continue; // 다음 단계 서버로 다시 루프
            }
            else
            {
                // 만약 IP가 없다면? 네임서버 이름을 다시 IP로 바꿔야 하는 복잡한 상황 발생 (Glue Record 부재)
                printf("     Error: Glue record missing for %s\n", ns_list.servers[0].name);
                return -1;
            }
        }

        printf("     Error: No answer and no further nameservers.\n");
        break;
    }

    return -1;
}