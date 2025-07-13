#include "myalloc.h"

#include <stdio.h>
#include <errno.h>

#include <sys/mman.h>

block_header_t head = {
    .size = 0,
    .free = true,
    .next = NULL,
    .prev = NULL,
    .block = NULL};

void print_block_header(block_header_t *header)
{
    if (header == NULL)
    {
        printf("Header is NULL\n");
        return;
    }
    printf("Block Header:\n");
    printf("  Size: %zu\n", header->size);
    printf("  Free: %s\n", header->free ? "true" : "false");
    printf("  Block Address: %p\n", header->block);
    printf("  Next: %p\n", header->next);
    printf("  Prev: %p\n", header->prev);
}

void print_stack()
{
    block_header_t *current = head.next;
    while (current != NULL)
    {
        print_block_header(current);
        current = current->next;
    }
}

void *alloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    size_t total_size = size + sizeof(block_header_t);

    // Look for a free block that has total_size or more available
    block_header_t *_p_bl = head.next;
    while (_p_bl != NULL)
    {
        if (_p_bl->free == true && _p_bl->size >= size)
        {
            break;
        }
        _p_bl = _p_bl->next;
    }

    if (_p_bl == NULL)
    {
        // Could not find a free block that is big enough, allocate new memory
        void *ptr = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (ptr == MAP_FAILED)
        {
            perror("mmap failed\n");
            ptr = NULL;
        }

        block_header_t *p_block = (block_header_t *)ptr;
        p_block->size = size;
        p_block->free = false;
        p_block->block = (void *)((char *)p_block + sizeof(block_header_t));
        p_block->prev = &head;
        p_block->next = head.next;
        if (head.next != NULL)
        {
            head.next->prev = p_block;
        }
        head.next = p_block;

        // print_block_header(p_block);

        return p_block->block;
    }

    // Found a free block that is big enough
    if (_p_bl->size < size)
    {
        // We will have space left after allocating the new block
        if (_p_bl->size - size <= sizeof(block_header_t))
        {
            // We cannot fit a header in the space that will be left so we can unmap that memory
            char *p_block_data = (char *)_p_bl->block;
            munmap(p_block_data + size, _p_bl->size - size);
        }
        else
        {
            // We will need to create a new block with the remaining space first and mark it as free
            size_t bytes_left = _p_bl->size - size;
            size_t data_left = bytes_left - sizeof(block_header_t);
            block_header_t *free_block = (block_header_t *)(((char *)_p_bl->block) + _p_bl->size);

            free_block->block = (void *)((char *)free_block + sizeof(block_header_t));
            free_block->free = true;
            free_block->next = _p_bl->next;
            _p_bl->next = free_block;
            free_block->prev = _p_bl;
            _p_bl->size -= bytes_left;
        }
        // print_block_header(_p_bl);
        return _p_bl->block;
    }
}

void dealloc(void *ptr)
{
    if (ptr == NULL)
    {
        perror("ptr is null\n");
        return;
    }

    // Look up the block header associated with ptr
    void *_ptr = (char *)ptr - sizeof(block_header_t);
    block_header_t *header = (block_header_t *)_ptr;

    size_t size = header->size;

    header->free = true;

    print_block_header(header);

    if ((header->prev == NULL || header->prev->free == false) &&
        (header->next == NULL || header->next->free == false))
    {
        printf("No coalesce\n");
        return;
    }
    else
    {
        printf("Coalesce\n");
    }

    if (header->prev != NULL && header->prev->free == true)
    {
        // Coalesce if possible
        // TODO: Check if this logic works
        if ((char *)header->prev + header->prev->size + sizeof(block_header_t) == (char *)header)
        {
            // We can include the size of the header in the new size as we do not need it anymore
            header->prev->size += header->size + sizeof(block_header_t);
            header->prev->next = header->next;

            if (header->next != NULL)
            {
                header->next->prev = header->prev;
            }
        }
    }
}
