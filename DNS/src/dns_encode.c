#include "dns_encode.h"
#include <stdint.h>
#include <string.h>

int build_dns_query(
    const char *domain,
    uint8_t *buffer)
{
    int pos = 0;
    int len_pos = 0;
    int count = 0;

    for (int i = 0; domain[i] != '\0'; i++)
    {
        if (domain[i] == '.')
        {
            buffer[len_pos] = count; // record length
            len_pos = pos + 1;       // store next length pos
            count = 0;               // reset count
        }
        else
        {
            buffer[pos + 1] = domain[i];
            count++;
        }
        pos++;
    }
    buffer[len_pos] = count;
    buffer[pos + 1] = 0x00; // end should be 0

    return pos + 2;
}

int encode_domain(const char *domain, uint8_t *buffer)
{
    int src_idx = 0; // 원본 도메인(domain)의 인덱스
    int dst_idx = 0; // 버퍼(buffer)의 인덱스
    int len_idx = 0; // '길이' 정보를 써넣을 위치 저장용
    int count = 0;   // 현재 레이블(문자열 덩어리)의 길이 카운트

    // 첫 번째 길이를 넣을 자리를 비워둡니다.
    dst_idx = 1;

    while (domain[src_idx] != '\0')
    {
        if (domain[src_idx] == '.')
        {
            // 점(.)을 만났을 때: 지금까지 센 길이를 비워둔 자리에 넣음
            buffer[len_idx] = (uint8_t)count;

            // 다음 레이블의 길이를 기록할 자리를 현재 위치로 갱신
            len_idx = dst_idx;
            dst_idx++;
            count = 0; // 카운트 초기화
        }
        else
        {
            // 일반 문자를 만났을 때: 버퍼에 복사하고 카운트 증가
            buffer[dst_idx] = (uint8_t)domain[src_idx];
            dst_idx++;
            count++;
        }
        src_idx++;
    }

    // 마지막 레이블(예: com)의 길이를 기록
    buffer[len_idx] = (uint8_t)count;

    // DNS 이름의 끝을 알리는 0(Null 테미네이터) 추가
    buffer[dst_idx] = 0x00;
    dst_idx++;

    return dst_idx; // 인코딩된 전체 데이터의 길이 반환
}