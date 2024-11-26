// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_avx512f_search() --- find DETI coins using md5_cpu_avx512f()
//

#ifndef DETI_COINS_CPU_AVX512F_SEARCH
#define DETI_COINS_CPU_AVX512F_SEARCH

#include <immintrin.h> // AVX512 intrinsics

static void deti_coins_cpu_avx512_search(void)
{
    u32_t n, idx, coin[13u], hash[4u];
    u64_t n_attempts, n_coins;
    u08_t *bytes;
    static u32_t interleaved_data[13u * 16u] __attribute__((aligned(64))); // 16 DETI coins
    static u32_t interleaved_hash[4u * 16u] __attribute__((aligned(64)));   // 16 MD5 hashes

    bytes = (u08_t *)&coin[0]; // acesses coin information byte per byte
    //
    // mandatory for a DETI coin
    //
    bytes[0u] = 'D';
    bytes[1u] = 'E';
    bytes[2u] = 'T';
    bytes[3u] = 'I';
    bytes[4u] = ' ';
    bytes[5u] = 'c';
    bytes[6u] = 'o';
    bytes[7u] = 'i';
    bytes[8u] = 'n';
    bytes[9u] = ' ';
    //
    // arbitrary, but printable utf-8 data terminated with a '\n' is highly desirable
    //
    for (idx = 10u; idx < 13u * 4u - 1u; idx++) // to do: generate random ASCII symbols
        bytes[idx] = ' ';
    //
    // mandatory termination
    //
    bytes[13u * 4u - 1u] = '\n';
    //
    // find DETI coins
    //
    for (n_attempts = n_coins = 0ul; stop_request == 0; n_attempts++)
    {
        //
        // interleave data for AVX-512 processing
        //
        for(idx = 0u; idx < 13u; idx++){
            interleaved_data[16u*idx] = coin[idx];
        }
        for(idx = 1u; idx < 16u;idx++){
            for(n = 0u; n < 13u; n++){
            interleaved_data[16u * n + idx] = interleaved_data[16u * n];
            }
        }

        //
        // compute MD5 hash using AVX-512
        //
        md5_cpu_avx512((v16si *)interleaved_data, (v16si *)interleaved_hash);  
        for(idx = 0u ; idx < 16u; idx++){
            for(n = 0u; n<4u; n++){
                hash[n] = interleaved_hash[16u*n + idx];
            }

            //
            // byte-reverse each word (that's how the MD5 message digest is printed...)
            //
            hash_byte_reverse(hash);
            //
            // count the number of trailing zeros of the MD5 hash
            //
            n = deti_coin_power(hash);
            //
            // if the number of trailing zeros is >= 32 we have a DETI coin
            //
            if (n >= 32u)
            {
                save_deti_coin(coin);
                n_coins++;
            }

        }

        //
        // try next combination (byte range: 0x20..0x7E)
        //
        for (idx = 10u; idx < 13u * 4u - 1u && bytes[idx] == (u08_t)126; idx++) // (u08_t)126 = ~ character
            bytes[idx] = ' ';
        if (idx < 13u * 4u - 1u)
            bytes[idx]++;
    }
    STORE_DETI_COINS();
    printf("deti_coins_cpu_avx512_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n", n_coins, (n_coins == 1ul) ? "" : "s", n_attempts, (n_attempts == 1ul) ? "" : "s", (double)n_attempts / (double)(1ul << 32));
}

#endif