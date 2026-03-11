/////////////////////////////////////////////////////////////////////////////////////////////
// 기본 제공코드는 임의 수정해도 관계 없습니다. 단, 입출력 포맷 주의
// 아래 표준 입출력 예제 필요시 참고하세요.
// 표준 입력 예제
// int a;
// float b, c;
// double d, e, f;
// char g;
// char var[256];
// long long AB;
// scanf("%d", &a);                      // int 변수 1개 입력받는 예제
// scanf("%f %f", &b, &c);               // float 변수 2개 입력받는 예제
// scanf("%lf %lf %lf", &d, &e, &f);     // double 변수 3개 입력받는 예제
// scanf("%c", &g);                      // char 변수 1개 입력받는 예제
// scanf("%s", &var);                    // 문자열 1개 입력받는 예제
// scanf("%lld", &AB);                   // long long 변수 1개 입력받는 예제
/////////////////////////////////////////////////////////////////////////////////////////////
// 표준 출력 예제
// int a = 0;
// float b = 1.0, c = 2.0;
// double d = 3.0, e = 0.0; f = 1.0;
// char g = 'b';
// char var[256] = "ABCDEFG";
// long long AB = 12345678901234567L;
// printf("%d", a);                      // int 변수 1개 출력하는 예제
// printf("%f %f", b, c);                // float 변수 2개 출력하는 예제
// printf("%lf %lf %lf", d, e, f);       // double 변수 3개 출력하는 예제
// printf("%c", g);                      // char 변수 1개 출력하는 예제
// printf("%s", var);                    // 문자열 1개 출력하는 예제
// printf("%lld", AB);                   // long long 변수 1개 출력하는 예제
/////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 10000

int SIZE; // (전역)맵 크기 N*N

int *map; // 1차원으로 하고 인덱스개산
int *dp;  // 똑같이

void input();
int solve();

void print_map();
void print_dp();
int get_min(int i, int j);

void set_free();

int main(void)
{
    int test_case;
    int T;
    /* 아래의 freopen 함수는 input.txt 를 read only 형식으로 연 후,
    앞으로 표준 입력(키보드) 대신 input.txt 파일로부터 읽어오겠다는 의미의 코드입니다.
    여러분이 작성한 코드를 테스트 할 때, 편의를 위해서 input.txt에 입력을 저장한 후,
    freopen 함수를 이용하면 이후 scanf 를 수행할 때 표준 입력 대신 파일로부터 입력을 받아올 수 있습니다.
    따라서 테스트를 수행할 때에는 아래 주석을 지우고 이 함수를 사용하셔도 좋습니다.
    단, 채점을 위해 코드를 제출하실 때에는 반드시 freopen 함수를 지우거나 주석 처리 하셔야 합니다.
    */
    // freopen("input.txt", "r", stdin);
    /* 아래 코드를 수행하지 않으면 여러분의 프로그램이 제한 시간 초과로 강제 종료 되었을 때,
    출력한 내용이 실제 표준 출력에 기록되지 않을 수 있습니다.
    따라서 안전을 위해 반드시 setbuf(stdout, NULL); 을 수행하시기 바랍니다.
    */
    setbuf(stdout, NULL);
    scanf("%d", &T);
    /*
    여러 개의 테스트 케이스를 처리하기 위한 부분입니다.
    */
    for (test_case = 1; test_case <= T; ++test_case)
    {
        input();
        printf("#%d %d\n", test_case, solve());
        set_free();
    }
    return 0; // 정상종료시 반드시 0을 리턴해야 합니다.
}

int solve()
{
    dp[0] = 0;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (i == 0 && j == 0)
                continue;

            dp[i * SIZE + j] = get_min(i, j);
        }
    }

    return dp[SIZE * SIZE - 1];
}

int get_min(int i, int j)
{
    int current = i * SIZE + j;
    int result = MAX;

    if (i > 0)
    {
        int up = (i - 1) * SIZE + j;
        int diff = (map[current] > map[up])
                       ? map[current] - map[up]
                       : 0;

        int cost = dp[up] + diff + 1;
        result = (cost < result) ? cost : result;
    }

    if (j > 0)
    {
        int left = i * SIZE + (j - 1);
        int diff = (map[current] > map[left])
                       ? map[current] - map[left]
                       : 0;

        int cost = dp[left] + diff + 1;
        result = (cost < result) ? cost : result;
    }

    return result;
}

void input()
{
    scanf("%d", &SIZE);

    map = malloc(sizeof(int) * SIZE * SIZE);
    dp = malloc(sizeof(int) * SIZE * SIZE);

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            scanf("%d", &map[i * SIZE + j]);
            dp[i * SIZE + j] = -1;
        }
    }
}

void print_map()
{
    printf("=== MAP ===\n");
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            printf("%d ", map[i * SIZE + j]);
        }
        printf("\n");
    }
}

void print_dp()
{
    printf("=== DP ===\n");
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            printf("%d ", dp[i * SIZE + j]);
        }
        printf("\n");
    }
}

void set_free()
{
    free(map);
    free(dp);
}