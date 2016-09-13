#include "merkle_tree_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void print_hashes(merkle_tree_memory* memory)
{
	for (unsigned i = 1; i < 32; i++)
	{
	printf(
		"0x%08x%08x%08x%08x%08x%08x%08x%08x\n",
		memory->buffer[i*8],
		memory->buffer[i*8+1],
		memory->buffer[i*8+2],
		memory->buffer[i*8+3],
		memory->buffer[i*8+4],
		memory->buffer[i*8+5],
		memory->buffer[i*8+6],
		memory->buffer[i*8+7]
	);
	}
}

int main(int argc, char** argv)
{
	unsigned v[8] = {0,1,2,3,4,5,6,7};
	unsigned z[8] = {0,0,0,0,0,0,0,0};
	printf("\n");
	printf("\n");
	merkle_tree_memory memory;
	merkle_tree_memory_init(&memory, 24);
	merkle_tree_recalculate_hashes(&memory);
  
  pcg32_random_t random;
  random.state = rand();
  random.inc = 1;

	printf("First calculation:\n");
	print_hashes(&memory);
	printf("\n");

	merkle_tree_memory_set(&memory, 8, v);

	printf("After invalidation:\n");
	print_hashes(&memory);
	printf("\n");
	
	merkle_tree_recalculate_hashes(&memory);
	
	printf("After update:\n");
	print_hashes(&memory);
	printf("\n");

	merkle_tree_memory_set(&memory, 0, z);
	merkle_tree_recalculate_hashes(&memory);
	
	printf("After re-zeroing:\n");
	print_hashes(&memory);
	printf("\n");
  
  unsigned long startTime, stopTime, hashStartTime, hashStopTime, hashTime;
	
  struct timeval tv;
  gettimeofday(&tv,NULL);
  hashTime = 0;
  startTime = 1000000 * tv.tv_sec + tv.tv_usec;
	for (unsigned j = 0; j < 15; j++)
	{
		for (unsigned i = 0; i < 1000000; i++)
		{
			merkle_tree_memory_set(&memory, i % memory.capacity, z);
		}
    gettimeofday(&tv,NULL);
    hashStartTime = 1000000 * tv.tv_sec + tv.tv_usec;
		merkle_tree_recalculate_hashes(&memory);
    gettimeofday(&tv,NULL);
    hashStopTime =  1000000 * tv.tv_sec + tv.tv_usec;
    hashTime += hashStopTime - hashStartTime;
	}
  gettimeofday(&tv,NULL);
  stopTime = 1000000 * tv.tv_sec + tv.tv_usec;
  
  fprintf(stderr, "Sequential trial took %0.5f seconds, of which %0.5f us hashing.\n", ((float) (stopTime - startTime)) / 1000000.0, ((float) hashTime) / 1000000.0);
	
  gettimeofday(&tv,NULL);
  hashTime = 0;
  startTime = 1000000 * tv.tv_sec + tv.tv_usec;
	for (unsigned j = 0; j < 15; j++)
	{
		for (unsigned i = 0; i < 1000000; i++)
		{
			merkle_tree_memory_set(&memory, pcg32_random_r(&random) % memory.capacity, z);
		}
    gettimeofday(&tv,NULL);
    hashStartTime = 1000000 * tv.tv_sec + tv.tv_usec;
		merkle_tree_recalculate_hashes(&memory);
    gettimeofday(&tv,NULL);
    hashStopTime =  1000000 * tv.tv_sec + tv.tv_usec;
    hashTime += hashStopTime - hashStartTime;
	}
  gettimeofday(&tv,NULL);
  stopTime = 1000000 * tv.tv_sec + tv.tv_usec;
  
  fprintf(stderr, "Random trial took %0.5f seconds, of which %0.5f hashing.\n", ((float) (stopTime - startTime)) / 1000000.0, ((float) hashTime) / 1000000.0);
	
  gettimeofday(&tv,NULL);
  hashTime = 0;
  startTime = 1000000 * tv.tv_sec + tv.tv_usec;
	for (unsigned j = 0; j < 15; j++)
	{
		for (unsigned i = 0; i < 1000000; i++)
		{
			merkle_tree_memory_set(&memory, pcg32_random_r(&random) % 524288, z);
		}
    gettimeofday(&tv,NULL);
    hashStartTime = 1000000 * tv.tv_sec + tv.tv_usec;
		merkle_tree_recalculate_hashes(&memory);
    gettimeofday(&tv,NULL);
    hashStopTime =  1000000 * tv.tv_sec + tv.tv_usec;
    hashTime += hashStopTime - hashStartTime;
	}
  gettimeofday(&tv,NULL);
  stopTime = 1000000 * tv.tv_sec + tv.tv_usec;
  
  fprintf(stderr, "Random in first 16MB took %0.5f seconds, of which %0.5f hashing.\n", ((float) (stopTime - startTime)) / 1000000.0, ((float) hashTime) / 1000000.0);
}
