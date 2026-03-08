#include "prime_gen.h"
#include <stdlib.h>
#include <time.h>

/* (a * b) % mod overflow 방지 */
ull mod_mul(ull a, ull b, ull mod)
{
    ull result = 0;
    a %= mod;

    while (b)
    {
        if (b & 1)
            result = (result + a) % mod;

        a = (a * 2) % mod;
        b >>= 1;
    }

    return result;
}

/* (base^exp) % mod */
ull mod_pow(ull base, ull exp, ull mod)
{
    ull result = 1;
    base %= mod;

    while (exp)
    {
        if (exp & 1)
            result = mod_mul(result, base, mod);

        base = mod_mul(base, base, mod);
        exp >>= 1;
    }

    return result;
}

/* Miller-Rabin 테스트 */
int miller_rabin(ull n, ull a)
{
    ull d = n - 1;
    int s = 0;

    while ((d & 1) == 0)
    {
        d >>= 1;
        s++;
    }

    ull x = mod_pow(a, d, n);

    if (x == 1 || x == n - 1)
        return 1;

    for (int i = 0; i < s - 1; i++)
    {
        x = mod_mul(x, x, n);

        if (x == n - 1)
            return 1;
    }

    return 0;
}

int is_prime(ull n)
{
    if (n < 2)
        return 0;

    ull test_primes[] = {2,3,5,7,11,13,17,19,23,29,31,37};

    for (int i = 0; i < 12; i++)
    {
        ull p = test_primes[i];

        if (n == p)
            return 1;

        if (n % p == 0)
            return 0;
    }

    ull bases[] = {2,325,9375,28178,450775,9780504,1795265022};

    for (int i = 0; i < 7; i++)
    {
        if (!miller_rabin(n, bases[i] % n))
            return 0;
    }

    return 1;
}

static ull get_big_odd()
{
    ull candidate;

    do
    {
        candidate = ((ull)rand() << 32) | rand();
        candidate |= 1;
    }
    while (candidate < 1000000000ULL);

    return candidate;
}

ull gen_prime()
{
    static int init = 0;

    if (!init)
    {
        srand(time(NULL));
        init = 1;
    }

    while (1)
    {
        ull n = get_big_odd();

        if (is_prime(n))
            return n;
    }
}