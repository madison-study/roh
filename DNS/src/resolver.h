#ifndef RESOLVER_H
#define RESOLVER_H

#define MAX_DOMAIN_LEN 256
#define MAX_IP_LEN 16

int resolve_domain(const char *domain, char *result_ip);

#endif