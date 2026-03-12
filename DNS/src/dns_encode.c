#include "dns_encode.h"

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
    buffer[pos + 1] = 0x00;

    return pos + 2;
}

int encode_domain(
    const char *domain,
    uint8_t *buffer)
{
}
