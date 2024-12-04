//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// MD5 hash CUDA kernel code
//
// deti_coins_cuda_kernel_search --- each thread computes the MD5 hash of one message
//
// do not use this directy to search for DETI coins!
//

//
// needed stuff
//

typedef unsigned int u32_t;

#include "md5.h"


extern "C" __global__ __launch_bounds__(128,1) void deti_coins_cuda_kernel_search(u32_t *storage_area,u32_t v1,u32_t v2)
{
  u32_t n,a,b,c,d,state[4],x[16],coin[13],hash[4];

  n = (u32_t)threadIdx.x + (u32_t)blockDim.x * (u32_t)blockIdx.x;
  coin[ 0] = 0x49544544u; // "DETI"
  coin[ 1] = 0x696f6320u; // "coi "
  coin[ 2] = 0x6e20206eu; // "n  n"
  coin[ 3] = 0x20202020u; 
  coin[ 4] = 0x20202020u; 
  coin[ 5] = 0x20202020u; 
  coin[ 6] = 0x20202020u; 
  coin[ 7] = 0x20202020u; 
  coin[ 8] = 0x20202020u; 
  coin[ 9] = 0x20202020u; 
  coin [10] = v1;
  coin [11] = v2;
  coin [12] = 0x20202020u; // "71\n  "
  coin [4] += (n % 64) << 0; n/=64;
  coin [4] += (n % 64) << 8; n/=64;
  coin [4] += (n % 64) << 16; n/=64; 
  coin [4] += (n % 64) << 24;
  for(n = 0; n < 64; n++){ 

  
  
  //
  // compute MD5 hash
  //
# define C(c)         (c)
# define ROTATE(x,n)  (((x) << (n)) | ((x) >> (32 - (n))))
# define DATA(idx)    coin[(idx)]
# define HASH(idx)    hash[(idx)]
# define STATE(idx)   state[idx]
# define X(idx)       x[idx]
     CUSTOM_MD5_CODE();
    if(hash[3] == 0){
      u32_t n = atomicAdd(storage_area,13);
      if(n + 13 <= 1024){
        for(int i = 0; i < 13; i++){
          storage_area[n+i] = coin[i];
        }
      } 
    }
    coin[12] += 1<<16;
  } 
}
