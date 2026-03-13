#include "dns_encode.h"
#include <stdint.h>
#include <string.h>

int encode_domain(const char *domain, uint8_t *buffer)
{
    int src_idx = 0; // Index for the original domain string
    int dst_idx = 0; // Index for the destination buffer
    int len_idx = 0; // Position where the length of the current label is stored
    int count = 0;   // Character count of the current label

    // Start filling the buffer from index 1, 
    // leaving index 0 for the first label's length.
    dst_idx = 1;

    while (domain[src_idx] != '\0')
    {
        if (domain[src_idx] == '.')
        {
            // When a dot is met, write the length of the label just finished
            buffer[len_idx] = (uint8_t)count;

            // Mark the current position as the spot for the next label's length
            len_idx = dst_idx;
            dst_idx++;
            
            // Reset character count for the next label
            count = 0;
        }
        else
        {
            // Copy character from domain string to buffer
            buffer[dst_idx] = (uint8_t)domain[src_idx];
            dst_idx++;
            count++;
        }
        src_idx++;
    }

    // Write the length of the last label (the part after the last dot)
    buffer[len_idx] = (uint8_t)count;

    // DNS name field must end with a null byte (zero-length label)
    buffer[dst_idx] = 0x00;
    dst_idx++;

    // Return the total size of the encoded domain name in bytes
    return dst_idx;
}