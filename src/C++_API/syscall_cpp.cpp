#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../lib/mem.h"

void* operator new (size_t size)
{
    //return __mem_alloc(size);
    return mem_alloc(size);
}

void* operator new[] (size_t size)
{
    //return __mem_alloc(size);
    return mem_alloc(size);
}

void operator delete (void* ptr)
{
    //__mem_free(ptr);
    mem_free(ptr);
}

void operator delete[] (void* ptr)
{
    //__mem_free(ptr);
    mem_free(ptr);
}