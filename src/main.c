#include "../lib/console.h"
#include "C_API/syscall_c.h"

typedef struct Block
{
    size_t size;
    struct Block* prev;
    struct Block* next;
} Block;
void main()
{
    size_t* test = __mem_alloc(70);
    *test = sizeof (size_t);
    __mem_free(test);
}