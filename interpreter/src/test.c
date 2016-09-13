#include "merkle_tree_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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
	merkle_tree_memory_init(&memory, 13);
	merkle_tree_recalculate_hashes(&memory);

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
	for (unsigned j = 0; j < 100; j++)
	{
		for (unsigned i = 0; i < 10000; i++)
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
  
  fprintf(stderr, "Sequential setting trial took %0.5f seconds, of which %0.5f us hashing.\n", ((float) (stopTime - startTime)) / 1000000.0, ((float) hashTime) / 1000000.0);
	
  gettimeofday(&tv,NULL);
  hashTime = 0;
  startTime = 1000000 * tv.tv_sec + tv.tv_usec;
	for (unsigned j = 0; j < 100; j++)
	{
		for (unsigned i = 0; i < 10000; i++)
		{
			merkle_tree_memory_set(&memory, rand() % memory.capacity, z);
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
  
  fprintf(stderr, "Random setting trial took %0.5f seconds, of which %0.5f hashing.\n", ((float) (stopTime - startTime)) / 1000000.0, ((float) hashTime) / 1000000.0);
	
  gettimeofday(&tv,NULL);
  hashTime = 0;
  startTime = 1000000 * tv.tv_sec + tv.tv_usec;
	for (unsigned j = 0; j < 100; j++)
	{
		for (unsigned i = 0; i < 10000; i++)
		{
			merkle_tree_memory_set(&memory, (int) sqrtf(rand() % (memory.capacity * memory.capacity)), z);
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
  
  fprintf(stderr, "Skewed setting trial took %0.5f seconds, of which %0.5f hashing.\n", ((float) (stopTime - startTime)) / 1000000.0, ((float) hashTime) / 1000000.0);
}
