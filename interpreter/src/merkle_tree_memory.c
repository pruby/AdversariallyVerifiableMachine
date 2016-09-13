#include "merkle_tree_memory.h"
#include "sha3.h"

#include <string.h>
#include <assert.h>

void merkle_tree_memory_init(merkle_tree_memory *memory, uint32_t addressBits)
{
  assert(addressBits <= 24);
	memory->capacity = 1 << addressBits;
	memory->buffer = malloc(32 * 2 * memory->capacity);
	memset(memory->buffer, 0, 32 * 2 * memory->capacity);
}

void merkle_tree_memory_free(merkle_tree_memory *memory)
{
	free(memory->buffer);
}

void merkle_tree_invalidate_hash(merkle_tree_memory *memory, uint32_t address)
{
	address = address + memory->capacity;
	while (address > 1)
	{
		address = address / 2;
		uint32_t i;
		uint32_t zero = 1;
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

void merkle_tree_memory_set(merkle_tree_memory *memory, uint32_t address, const uint32_t value[8])
{
  address = (address / 32) & (memory->capacity - 1);
	uint32_t i;
	for (i = 0; i < 8; ++i)
	{
		memory->buffer[(memory->capacity + address) * 8 + i] = value[i];
	}
  
  merkle_tree_invalidate_hash(memory, address);
}

void merkle_tree_memory_set32(merkle_tree_memory *memory, uint32_t address, uint32_t value)
{
  address = (address / 4) & ((memory->capacity * 8) - 1);
	uint32_t i;
	memory->buffer[memory->capacity * 8 + address + i] = value;
  
  merkle_tree_invalidate_hash(memory, address / 8);
}

void merkle_tree_memory_setByte(merkle_tree_memory *memory, uint32_t address, uint8_t value)
{
  address = address & ((memory->capacity * 32) - 1);
	((uint8_t*) memory->buffer)[memory->capacity * 32 + address] = value;
  
  merkle_tree_invalidate_hash(memory, address / 32);
}

void merkle_tree_memory_get(const merkle_tree_memory *memory, uint32_t address, uint32_t out[8])
{
  address = (address / 32) & (memory->capacity - 1);
	uint32_t i;
	for (i = 0; i < 8; ++i)
	{
		out[i] = memory->buffer[(memory->capacity + address) * 8 + i];
	}
}

uint32_t merkle_tree_memory_get32(const merkle_tree_memory *memory, uint32_t address)
{
  address = (address / 4) & ((memory->capacity * 8) - 1);
	return memory->buffer[memory->capacity * 8 + address];
}

uint8_t merkle_tree_memory_getByte(const merkle_tree_memory *memory, uint32_t address)
{
  address = address & ((memory->capacity * 32) - 1);
	return ((uint8_t*) memory->buffer)[memory->capacity * 32 + address];
}

void merkle_tree_recalculate_hashes(merkle_tree_memory *memory)
{
	uint32_t i;
	uint32_t c;
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

