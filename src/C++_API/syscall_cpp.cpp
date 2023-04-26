#include "../../h/C++_API/syscall_cpp.hpp"

void* operator new (size_t size)
{
    return mem_alloc(size);
}

void* operator new[] (size_t size)
{
    return mem_alloc(size);
}

void operator delete (void* ptr)
{
    mem_free(ptr);
}

void operator delete[] (void* ptr)
{
    mem_free(ptr);
}