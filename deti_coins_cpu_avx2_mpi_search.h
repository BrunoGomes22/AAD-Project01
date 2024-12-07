// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_avx2_mpi_search() --- find DETI coins using md5_cpu_avx2() with MPI
//

#if USE_MPI > 0
#ifndef DETI_COINS_CPU_AVX2_MPI_SEARCH
#define DETI_COINS_CPU_AVX2_MPI_SEARCH

#include <mpi.h>

static void deti_coins_cpu_avx2_mpi_search(void)
{
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    u32_t n, idx, coin[13u], hash[4u];
    u64_t n_attempts = 0, n_coins = 0;
    u08_t *bytes;
    u32_t interleaved_coins[13u * 8u] __attribute__((aligned(32))); // 8 DETI coins
    u32_t interleaved_hash[4u * 8u] __attribute__((aligned(32)));   // 8 MD5 hashes

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

    bytes[10u] = ' ';
    bytes[11u] = ' ';
    bytes[12u] = ' ';
    bytes[13u] = ' ';
    bytes[14u] = ' ';
    bytes[15u] = ' ';

    bytes[16u] = 's';
    bytes[17u] = 'e';
    bytes[18u] = 'a';
    bytes[19u] = 'r';
    bytes[20u] = 'c';
    bytes[21u] = 'h';
    bytes[22u] = '_';
    bytes[23u] = 'i';
    bytes[24u] = 'd';
    bytes[25u] = '=';

    bytes[26u] = '[';
    bytes[27u] = random_str[0];
    bytes[28u] = random_str[1];
    bytes[29u] = random_str[2];
    bytes[30u] = random_str[3];
    bytes[31u] = ']';

    for (idx = 32u; idx < 13u * 4u - 1u; idx++)
        bytes[idx] = ' ';
    //
    // mandatory termination
    //
    bytes[13u * 4u - 1u] = '\n';
    //
    // find DETI coins
    //
    coin[12] += rank;
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

    u64_t total_coins = 0, total_attempts = 0;
    MPI_Reduce(&n_coins, &total_coins, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&n_attempts, &total_attempts, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0){
        printf("total_coins: %lu\n", total_coins);
        printf("total_attempts: %lu\n", total_attempts);
    }

    STORE_DETI_COINS();

    MPI_Finalize();
}

#endif
#endif