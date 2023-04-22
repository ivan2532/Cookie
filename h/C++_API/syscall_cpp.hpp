#ifndef _syscall_cpp_
#define _syscall_cpp_

#include "../C_API/syscall_c.h"

void* operator new (size_t size);
void* operator new[] (size_t size);
void operator delete (void* ptr);
void operator delete[] (void* ptr);

#endif
