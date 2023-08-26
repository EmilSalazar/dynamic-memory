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

void *noFree_Malloc(size_t size)
{
    void *p = sbrk(0);
    void *request = sbrk(size);
    if (request == (void *)-1)
    {
        return NULL;
    }
    else
    {
        return p;
    }
}

struct block_meta
{
    size_t size;
    struct block_meta *next;
    struct block_meta *prev; // new previous pointer
    int Free;
    int magic;
};

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;

void print_memory_leak() 
{
    struct block_meta *current = global_base;
    size_t total_allocated = 0;
    size_t actual_allocated = 0;
    while (current != NULL) 
    {
        total_allocated += current->size + META_SIZE;
        if (!current->Free) 
        {
            actual_allocated += current->size;
        }
        current = current->next;
    }

    if (actual_allocated < total_allocated) 
    {
        size_t leak_size = total_allocated - actual_allocated; //(start - end) - actual_allocated; //
        printf("Memory leak detected: %lu bytes\n", leak_size);
    } 
    else 
    {
        printf("No memory leak detected\n");
    }
}

struct block_meta *find_Free_block(struct block_meta **last, size_t size)
{
    struct block_meta *current = global_base;
    struct block_meta *best_fit = NULL;
    size_t smallest_size = (size_t)-1; // initialize to maximum value

    while (current) {
        if (current->Free && current->size >= size && current->size < smallest_size) {
            best_fit = current;
            smallest_size = current->size;
        }
        *last = current;
        current = current->next;
    }

    return best_fit;
}

struct block_meta *request_space(struct block_meta *last, size_t size)
{
    struct block_meta *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    assert((void *)block == request);
    if (request == (void *)-1)
    {
        return NULL;
    }

    if (last)
    {
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->prev = last;
    block->Free = 0;
    block->magic = 0x12345678;
    return block;
}

void *Malloc(size_t size)
{
    struct block_meta *block;

    if (size <= 0)
    {
        return NULL;
    }

    if (!global_base)
    {
        block = request_space(NULL, size);
        if (!block)
        {
            return NULL;
        }
        global_base = block;
    }
    else
    {
        struct block_meta *last = global_base;
        block = find_Free_block(&last, size);
        if (!block)
        {
            block = request_space(last, size);
            if (!block)
            {
                return NULL;
            }
        }
        else
        {
            block->Free = 0;
            block->magic = 0x77777777;
        }
    }

    return (block + 1);
}

void *Calloc(size_t nelem, size_t elsize)
{
    size_t size = nelem * elsize;
    void *ptr = Malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

struct block_meta *get_block_ptr(void *ptr)
{
    return (struct block_meta *)ptr - 1;
}

void Free(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    struct block_meta *block_ptr = get_block_ptr(ptr);
    assert(block_ptr->Free == 0);
    assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->Free = 1;
    block_ptr->magic = 0x55555555;

    // update previous pointer of next block
    if (block_ptr->next)
    {
        block_ptr->next->prev = block_ptr->prev;
    }
    // update next pointer of previous block
    if (block_ptr->prev)
    {
        block_ptr->prev->next = block_ptr->next;
    }
    else
    {
        global_base = block_ptr->next;
    }

    // check if we can release memory to the OS
    if (block_ptr->prev && block_ptr->prev->Free)
    {
        block_ptr = block_ptr->prev;
        sbrk(0 - META_SIZE - block_ptr->size);
        block_ptr->size += META_SIZE + block_ptr->next->size;
        block_ptr->next = block_ptr->next->next;
        if (block_ptr->next)
        {
            block_ptr->next->prev = block_ptr;
        }
    }
    else if (!block_ptr->prev)
    {
        if (block_ptr->next)
        {
            block_ptr->next->prev = NULL;
            global_base = block_ptr->next;
        }
        else
        {
            global_base = NULL;
        }
        sbrk(0 - META_SIZE - block_ptr->size);
    }
}

void *Realloc(void *ptr, size_t size)
{
    if (!ptr)
    {
        // NULL pointer, behave like Malloc()
        return Malloc(size);
    }
    struct block_meta *block_ptr = get_block_ptr(ptr);
    if (size == 0)
    {
        Free(ptr);
        return NULL;
    }
    // TODO; implement the rest of the function
}

