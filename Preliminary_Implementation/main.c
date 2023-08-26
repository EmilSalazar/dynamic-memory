#include <stdio.h>
#include "header.h"

void print_heap_info()
{
    printf("%p\n", sbrk(0));
}

int main()
{
    printf("Heap Start: ");
    print_heap_info();
    printf("\n--------Malloc--------\n");
    int mallocNumbers[] = {161, 180, 13, 72, 127, 86, 180, 137, 56, 272};
    void *ptr1 = Malloc(mallocNumbers[0]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[0], ptr1);
    void *ptr2 = Malloc(mallocNumbers[1]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[1], ptr2);
    void *ptr3 = Malloc(mallocNumbers[2]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[2], ptr3);
    void *ptr4 = Malloc(mallocNumbers[3]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[3], ptr4);
    void *ptr5 = Malloc(mallocNumbers[4]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[4], ptr5);
    void *ptr6 = Malloc(mallocNumbers[5]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[5], ptr6);
    void *ptr7 = Malloc(mallocNumbers[6]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[6], ptr7);
    void *ptr8 = Malloc(mallocNumbers[7]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[7], ptr8);
    void *ptr9 = Malloc(mallocNumbers[8]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[8], ptr9);
    void *ptr10 = Malloc(mallocNumbers[9]);
    printf("Allocated %d bytes at %p\n", mallocNumbers[9], ptr10);

    printf("\n--------Calloc--------\n");
    int callocNumbers[] = {20, 69, 16, 42, 5, 13, 7, 48}; //must been in even number since its two inputs for each calloc call
    void *ptr11 = Calloc(callocNumbers[0], callocNumbers[1]);
    printf("Allocated %d bytes at %p\n", (callocNumbers[0] * callocNumbers[1]), ptr11);
    void *ptr12 = Calloc(callocNumbers[2], callocNumbers[3]);
    printf("Allocated %d bytes at %p\n", (callocNumbers[2] * callocNumbers[3]), ptr12);
    void *ptr13 = Calloc(callocNumbers[4], callocNumbers[5]);
    printf("Allocated %d bytes at %p\n", (callocNumbers[4] * callocNumbers[5]), ptr13);
    void *ptr14 = Calloc(callocNumbers[6], callocNumbers[7]);
    printf("Allocated %d bytes at %p\n", (callocNumbers[6] * callocNumbers[7]), ptr14);

    printf("\n--------Realloc--------\n");
    int reallocNumbers[] = {269, 12};
    void *ptr21 = Realloc(ptr1, reallocNumbers[0]);
    printf("Allocated %d bytes at %p\n", reallocNumbers[0], ptr21);
    void *ptr22 = Realloc(ptr2, reallocNumbers[1]);
    printf("Allocated %d bytes at %p\n", reallocNumbers[1], ptr22);

    printf("\n--------Free--------\n");
    Free(ptr11);
    printf("Freed pointer at %p\n", ptr11);
    Free(ptr12);
    printf("Freed pointer at %p\n", ptr12);
    Free(ptr6);
    printf("Freed pointer at %p\n", ptr6);
    Free(ptr7);
    printf("Freed pointer at %p\n", ptr7);

    printf("\nHeap End: ");
    print_heap_info();
    print_memory_leak();
    return 0;
}