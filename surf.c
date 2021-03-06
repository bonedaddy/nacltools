/*
%use /usr/lib/randombytes.o;
*/
/* Public Domain */
#include <stdint.h>
#include <nacl/randombytes.h>
#include "surf.h"
#include "byte.h"

static uint32_t seed[32];
static uint32_t in[12];
static uint32_t out[8];
static int surf_init_done = 0;
static int outleft = 0;

#define ROTATE(x,b) (((x) << (b)) | ((x) >> (32 - (b))))
#define MUSH(i,b) x = t[i] += (((x ^ seed[i]) + sum) ^ ROTATE(x,b));
static void surf()
{
  uint32_t t[12]; uint32_t x; uint32_t sum = 0;
  int r; int i; int loop;

  for (i = 0;i < 12;++i) t[i] = in[i] ^ seed[12 + i];
  for (i = 0;i < 8;++i) out[i] = seed[24 + i];
  x = t[11];
  for (loop = 0;loop < 2;++loop) {
    for (r = 0;r < 16;++r) {
      sum += 0x9e3779b9;
      MUSH(0,5) MUSH(1,7) MUSH(2,9) MUSH(3,13)
      MUSH(4,5) MUSH(5,7) MUSH(6,9) MUSH(7,13)
      MUSH(8,5) MUSH(9,7) MUSH(10,9) MUSH(11,13)
    }
    for (i = 0;i < 8;++i) out[i] ^= t[i + 4];
  }
}

void surf_init()
{
    int i;
    
    /* Never need to call this twice */
    if(surf_init_done!=0) return;

    /* Create 128-byte seed */
    randombytes((unsigned char*)seed,128);

    /* Zeroize initial 48-byte "in" */
    for(i=0;i<12;i++) { in[i]=0; }

    /* Mark that surf_init has been completed */
    surf_init_done = 1;
}

void surf_randombytes(unsigned char *x,unsigned long long xlen)
{
  if(!surf_init_done) surf_init();
  while (xlen > 0) {
    if (!outleft) {
      if (!++in[0]) if (!++in[1]) if (!++in[2]) ++in[3];
      surf();
      outleft = 32;
    }
    *x = ((unsigned char*)(out))[--outleft];
    ++x;
    --xlen;
  }
}
