#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
// Don't include stdlb since the names will conflict?

// TODO; align

// sbrk some extra space every time we need it.
// This does no bookkeeping and therefore has no ability to Free, Realloc, etc.
void *noFree_Malloc(size_t size) {
  void *p = sbrk(0);
  void *request = sbrk(size);
  if (request == (void*) -1) { 
    return NULL; // sbrk failed
  } else {
    assert(p == request); // Not thread safe.
    return p;
  }
}

struct block_meta {
  size_t size;
  struct block_meta *next;
  int Free;
  int magic;  // For debugging only. TODO; remove this in non-debug mode.
};

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;

// Iterate through blocks until we find one that's large enough.
// TODO; split block up if it's larger than necessary
struct block_meta *find_Free_block(struct block_meta **last, size_t size) {
  struct block_meta *current = global_base;
  while (current && !(current->Free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}

struct block_meta *request_space(struct block_meta* last, size_t size) {
  struct block_meta *block;
  block = sbrk(0);
  void *request = sbrk(size + META_SIZE);
  assert((void*)block == request); // Not thread safe.
  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }
  
  if (last) { // NULL on first request.
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->Free = 0;
  block->magic = 0x12345678;
  return block;
}

// If it's the first ever call, i.e., global_base == NULL, request_space and set global_base.
// Otherwise, if we can find a Free block, use it.
// If not, request_space.
void *Malloc(size_t size) {
  struct block_meta *block;
  // TODO; align size?

  if (size <= 0) {
    return NULL;
  }

  if (!global_base) { // First call.
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    global_base = block;
  } else {
    struct block_meta *last = global_base;
    block = find_Free_block(&last, size);
    if (!block) { // Failed to find Free block.
      block = request_space(last, size);
      if (!block) {
	return NULL;
      }
    } else {      // Found Free block
      // TODO; consider splitting block here.
      block->Free = 0;
      block->magic = 0x77777777;
    }
  }
  
  return(block+1);
}

void *Calloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize;
  void *ptr = Malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

// TODO; maybe do some validation here.
struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

void Free(void *ptr) {
  if (!ptr) {
    return;
  }

  // TODO; consider merging blocks once splitting blocks is implemented.
  struct block_meta* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->Free == 0);
  assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
  block_ptr->Free = 1;
  block_ptr->magic = 0x55555555;  
}

void *Realloc(void *ptr, size_t size) {
  if (!ptr) { 
    // NULL ptr. Realloc should act like Malloc.
    return Malloc(size);
  }

  struct block_meta* block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size) {
    // We have enough space. Could Free some once we implement split.
    return ptr;
  }

  // Need to really Realloc. Malloc new space and Free old space.
  // Then copy old data to new space.
  void *new_ptr;
  new_ptr = Malloc(size);
  if (!new_ptr) {
    return NULL; // TODO; set errno on failure.
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  Free(ptr);  
  return new_ptr;
}

void print_memory_leak()
{
    struct block_meta *current = global_base;
    size_t total_allocated_memory = 0;
    size_t actual_allocated_memory = 0;

    while (current != NULL) 
    {
        total_allocated_memory += current->size;
        if (current->Free == 0) 
        {
            actual_allocated_memory += current->size;
        }
        current = current->next;
    }
    if (actual_allocated_memory < total_allocated_memory) 
    {
        size_t memory_leak = total_allocated_memory - actual_allocated_memory;
        printf("Memory leak detected: %zu bytes\n", memory_leak);
    } 
    else 
    {
        printf("No memory leak detected.\n");
    }
}
