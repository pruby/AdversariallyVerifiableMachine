#ifndef _merkle_tree_memory
#define _merkle_tree_memory

#include <stdint.h>

typedef struct
{
  uint32_t capacity;
  uint8_t temp[64];
  uint32_t *buffer;
} merkle_tree_memory;

void merkle_tree_memory_init(merkle_tree_memory *memory, uint32_t addressBits);
void merkle_tree_memory_free(merkle_tree_memory *memory);
void merkle_tree_invalidate_hash(merkle_tree_memory *memory, uint32_t address);
void merkle_tree_memory_set(merkle_tree_memory *memory, uint32_t address, const uint32_t value[8]);
void merkle_tree_memory_set32(merkle_tree_memory *memory, uint32_t address, uint32_t value);
void merkle_tree_memory_setByte(merkle_tree_memory *memory, uint32_t address, uint8_t value);
void merkle_tree_memory_get(const merkle_tree_memory *memory, uint32_t address, uint32_t out[8]);
uint32_t merkle_tree_memory_get32(const merkle_tree_memory *memory, uint32_t address);
uint8_t merkle_tree_memory_getByte(const merkle_tree_memory *memory, uint32_t address);
void merkle_tree_recalculate_hashes(merkle_tree_memory *memory);

#endif

