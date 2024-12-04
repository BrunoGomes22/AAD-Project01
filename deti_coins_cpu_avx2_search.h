// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_avx2_search() --- find DETI coins using md5_cpu_avx2()
//

#ifndef DETI_COINS_CPU_AVX2_SEARCH
#define DETI_COINS_CPU_AVX2_SEARCH

static void deti_coins_cpu_avx2_search(void)
{
    u32_t n, idx, coin[13u], hash[4u];
    u64_t n_attempts, n_coins;
    u08_t *bytes;
    static u32_t interleaved_coins[13u * 8u] __attribute__((aligned(32))); // 8 DETI coins
    static u32_t interleaved_hash[4u * 8u] __attribute__((aligned(32)));   // 8 MD5 hashes

    bytes = (u08_t *)&coin[0]; // accesses coin information byte per byte
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
    u32_t random_num = rand() % 10000;

    char random_str[5];
    snprintf(random_str, sizeof(random_str), "%04u", random_num);

    bytes[10u] = random_str[0];
    bytes[11u] = random_str[1];
    bytes[12u] = random_str[2];
    bytes[13u] = random_str[3];

    for (idx = 14u; idx < 13u * 4u - 1u; idx++)
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
        // interleave data for AVX2 processing
        //
        for(idx = 0;idx < 8;idx++){
            for(n = 0;n < 12;n++){
                interleaved_coins[8*n+idx] = coin[n];
            }
            interleaved_coins[8*n+idx] = coin[n] + idx; // change last block of the coin to introduce variability
        }
        //
        // compute MD5 hash using AVX2
        //
        md5_cpu_avx2((v8si *)interleaved_coins, (v8si *)interleaved_hash);  
        for (idx = 0u; idx < 8u; idx++) {
            for (n = 0u; n < 4u; n++) {
                hash[n] = interleaved_hash[8u * n + idx];
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
            if (n >= 32u) {
                coin[12] += idx;
                save_deti_coin(coin);
                coin[12] -= idx;
                n_coins++;
            }
        }

        //
        // try next combination (byte range: 0x20..0x7E)
        //
        for (idx = 10u; idx < 13u * 4u - 1u && bytes[idx] == (u08_t)126; idx++)
            bytes[idx] = ' ';
        if (idx < 13u * 4u - 1u)
            bytes[idx]++;
    }
    STORE_DETI_COINS();
    printf("deti_coins_cpu_avx2_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n", n_coins, (n_coins == 1ul) ? "" : "s", n_attempts, (n_attempts == 1ul) ? "" : "s", (double)n_attempts / (double)(1ul << 32));
}

#endif