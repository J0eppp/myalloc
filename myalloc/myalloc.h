#ifndef MYALLOC_H
#define MYALLOC_H

#include <string.h>
#include <stdbool.h>

#define DEFAULT_BLOCK_SIZE 4096

typedef struct block_header
{
    size_t size;
    bool free;
    struct block_header *next;
    struct block_header *prev;
    void *block;
} block_header_t;

void print_block_header(block_header_t *header);

void print_stack();

void *alloc(size_t size);

void dealloc(void *ptr);

#endif // MYALLOC_H
