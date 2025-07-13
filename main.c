#include <stdio.h>
#include "myalloc/myalloc.h"

int main(int, char **)
{
    void *ptr = alloc(32);
    printf("Pointer address: %p\n", ptr);

    printf("Printing heap\n");
    print_stack();
    printf("Done printing heap\n");

    void *ptr2 = alloc(8);
    printf("Pointer address: %p\n", ptr2);

    printf("Printing heap\n");
    print_stack();
    printf("Done printing heap\n");


    printf("Deallocating pointers\n");
    dealloc(ptr);
    ptr = NULL;
    dealloc(ptr2);
    ptr2 = NULL;

    printf("Printing heap\n");
    print_stack();
    printf("Done printing heap\n");
}
