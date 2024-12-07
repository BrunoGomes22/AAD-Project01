// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_mpi_search() --- find DETI coins using md5_cpu() with MPI
//
#if USE_MPI > 0
#ifndef DETI_COINS_CPU_MPI_SEARCH
#define DETI_COINS_CPU_MPI_SEARCH

#include <mpi.h>

static void deti_coins_cpu_mpi_search(void)
{
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


  u32_t n,idx,coin[13u],hash[4u];
  u64_t n_attempts,n_coins;
  u08_t *bytes;

  bytes = (u08_t *)&coin[0];
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
  // arbitrary, but printable utf-8 data terminated with a '\n' is hightly desirable
  //
  for(idx = 10u;idx < 13u * 4u - 1u;idx++)
    bytes[idx] = ' ';
  //
  // mandatory termination
  //
  bytes[13u * 4u -  1u] = '\n';
  //
  // find DETI coins
  //
  coin[12] += rank;
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts++)
  {
    //
    // compute MD5 hash
    //
    md5_cpu(coin,hash);
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
    if(n >= 32u)
    {
      save_deti_coin(coin);
      n_coins++;
    }
    //
    // try next combination (byte range: 0x20..0x7E)
    //
    for(idx = 10u;idx < 13u * 4u - 1u && bytes[idx] == (u08_t)126;idx++)
      bytes[idx] = ' ';
    if(idx < 13u * 4u - 1u)
      bytes[idx]++;
  }
    printf("n_coins: %lu\n", n_coins);
    printf("total_attempts: %lu\n", n_attempts);

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