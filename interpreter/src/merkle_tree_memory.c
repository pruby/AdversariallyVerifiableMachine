#include "merkle_tree_memory.h"
#include "sha3.h"

#include <string.h>

void merkle_tree_memory_init(merkle_tree_memory *memory, unsigned addressBits)
{
	memory->capacity = 1 << addressBits;
	memory->buffer = malloc(32 * 2 * memory->capacity);
	memset(memory->buffer, 0, 32 * 2 * memory->capacity);
}

void merkle_tree_memory_free(merkle_tree_memory *memory)
{
	free(memory->buffer);
}

void merkle_tree_invalidate_hash(merkle_tree_memory *memory, unsigned address)
{
	address = address + memory->capacity;
	while (address > 1)
	{
		address = address / 2;
		unsigned i;
		unsigned zero = 1;
		for (i = 0; i < 8; ++i)
		{
			if (memory->buffer[8 * address + i] != 0)
			{
				zero = 0;
				memory->buffer[8 * address + i] = 0;
			}
		}
		
		if (zero)
		{
			// If already zeroed at this point, we will have done to back to the root
			return;
		}
	}
}

void merkle_tree_memory_set(merkle_tree_memory *memory, unsigned address, unsigned value[8])
{
	unsigned i;
	for (i = 0; i < 8; ++i)
	{
		memory->buffer[(memory->capacity + address) * 8 + i] = value[i];
	}
  
  merkle_tree_invalidate_hash(memory, address);
}

void merkle_tree_recalculate_hashes(merkle_tree_memory *memory)
{
	unsigned i;
	unsigned c;
	for (i = memory->capacity - 1; i >= memory->capacity / 2; i--)
	{
		if (!memory->buffer[i * 8])
		{
			c = i * 2 + 1;
			sha3_256(memory->temp + 32, 32, ((uint8_t*) &memory->buffer[c * 8]) + 32, 32);
			
			c = i * 2;
			sha3_256(memory->temp, 32, (uint8_t*) &memory->buffer[c * 8], 32);
			
			sha3_256((uint8_t*) &memory->buffer[i * 8], 32, memory->temp, 64);
		}
	}
	
	for (; i > 0; i--)
	{
		if (!memory->buffer[i * 8])
		{
			c = i * 2;
			sha3_256((uint8_t*) &memory->buffer[i * 8], 32, (uint8_t*) &memory->buffer[c * 8], 64);
		}
	}
}

