#include <stdio.h>
#include <stdlib.h>
#include "resolver.h"

int main(int argc, char *argv[]) {
    // 사용자가 도메인을 입력하지 않았을 경우 기본값 사용
    const char *target_domain = (argc > 1) ? argv[1] : "wisc.edu";
    char result_ip[MAX_IP_LEN];

    printf("Resolving domain: %s...\n", target_domain);

    // resolver 레이어를 통해 IP 주소 획득
    if (resolve_domain(target_domain, result_ip) == 0) {
        printf("Success! IP Address: %s\n", result_ip);
    } else {
        fprintf(stderr, "Error: Could not resolve domain '%s'\n", target_domain);
        return 1;
    }

    return 0;
}