#include <stdlib.h>
#ifndef SEARCH_UTILITIES
#define SEARCH_UTILITIES

typedef unsigned int u32_t;

u32_t next_value_to_try_ascii(u32_t v);

u32_t next_value_to_try_ascii(u32_t v) {
    do {
        v++;                                               
        if ((v & 0xFF) == 0x7F) { 
            v += 0xA1;                                          
            if (((v >> 8) & 0xFF) == 0x7F) { 
                v += 0xA1 << 8;
                if (((v >> 16) & 0xFF) == 0x7F) {
                    v += 0xA1 << 16;
                    if (((v >> 24) & 0xFF) == 0x7F) { 
                        v += 0xA1 << 24;
                    }
                }
            }
        }
    } while (0);

    return v;
}

// Function to generate a random 4-byte value in ASCII characters
u32_t random_value_to_try_ascii(void) {
    u32_t value = 0;
    for (int i = 0; i < 4; i++) {
        value |= ((rand() % (126 - 32 + 1)) + 32) << (8 * i);
    }
    return value;
}

void test_next_value_to_try_ascii(void) {
    u32_t value = 32; // Start from ASCII space
    for (int i = 0; i < 10000; i++) {
        value = next_value_to_try_ascii(value); // Simulate some iteration
    }
}

#endif 
