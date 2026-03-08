#include <stdio.h>
#include "prime_gen.h"

typedef unsigned long long ull;

/* gcd */
ull gcd(ull a, ull b)
{
    while (b)
    {
        ull t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/* Extended Euclidean Algorithm */
long long egcd(long long a, long long b, long long *x, long long *y)
{
    if (b == 0)
    {
        *x = 1;
        *y = 0;
        return a;
    }

    long long x1, y1;

    long long g = egcd(b, a % b, &x1, &y1);

    *x = y1;
    *y = x1 - y1 * (a / b);

    return g;
}

/* modular inverse */
ull mod_inverse(ull a, ull m)
{
    long long x, y;

    egcd(a, m, &x, &y);

    x %= m;

    if (x < 0)
        x += m;

    return x;
}

int main()
{
    /* 1. 소수 생성 */
    ull p = gen_prime();
    ull q = gen_prime();

    while (p == q)
        q = gen_prime();

    /* 2. n 계산 */
    ull n = p * q;

    /* 3. φ(n) */
    ull phi = (p - 1) * (q - 1);

    /* 4. 공개키 e */
    ull e = 65537;

    /* gcd 체크 */
    if (gcd(e, phi) != 1)
    {
        printf("e not valid\n");
        return 1;
    }

    /* 5. 개인키 d */
    ull d = mod_inverse(e, phi);

    printf("p = %llu\n", p);
    printf("q = %llu\n", q);
    printf("n = %llu\n\n", n);

    printf("Public key (n, e)\n");
    printf("(%llu, %llu)\n\n", n, e);

    printf("Private key (n, d)\n");
    printf("(%llu, %llu)\n\n", n, d);

    /* 메시지 */
    ull message = 12345;

    printf("Original message: %llu\n", message);

    /* 암호화 */
    ull cipher = mod_pow(message, e, n);

    printf("Encrypted: %llu\n", cipher);

    /* 복호화 */
    ull decrypted = mod_pow(cipher, d, n);

    printf("Decrypted: %llu\n", decrypted);

    return 0;
}