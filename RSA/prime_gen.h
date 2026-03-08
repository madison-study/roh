#ifndef PRIME_GEN_H
#define PRIME_GEN_H

#include <stdint.h>

typedef unsigned long long ull;

ull mod_mul(ull a, ull b, ull mod);
ull mod_pow(ull base, ull exp, ull mod);

int miller_rabin(ull n, ull a);
int is_prime(ull n);

ull gen_prime();

#endif