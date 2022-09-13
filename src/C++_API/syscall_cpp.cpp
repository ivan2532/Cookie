#include "syscall_cpp.h"

void* operator new (size_t size)
{
    return __mem_alloc(size);
}

void operator delete (void* ptr)
{
    __mem_free(ptr);
}
