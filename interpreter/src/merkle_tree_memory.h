#ifndef _merkle_tree_memory
#define _merkle_tree_memory

#include <stdint.h>

typedef struct
{
  unsigned capacity;
  uint8_t temp[64];
  unsigned *buffer;
} merkle_tree_memory;

void merkle_tree_memory_init(merkle_tree_memory *memory, unsigned addressBits);
void merkle_tree_memory_free(merkle_tree_memory *memory);
void merkle_tree_invalidate_hash(merkle_tree_memory *memory, unsigned address);
void merkle_tree_memory_set(merkle_tree_memory *memory, unsigned address, unsigned value[8]);
void merkle_tree_recalculate_hashes(merkle_tree_memory *memory);

#endif

