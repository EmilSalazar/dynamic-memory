/*#ifndef MY_Malloc_H
#define MY_Malloc_H*/

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *noFree_Malloc(size_t size);
void *Malloc(size_t size);
void *Calloc(size_t nelem, size_t elsize);
void Free(void *ptr);
void *Realloc(void *ptr, size_t size);
void print_memory_leak();

//#endif /* MY_Malloc_H */