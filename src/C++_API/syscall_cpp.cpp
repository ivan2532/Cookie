#include "../../h/C++_API/syscall_cpp.hpp"

void* operator new (size_t size)
{
    return mem_alloc(size);
}

void operator delete (void* ptr)
{
    mem_free(ptr);
}

Thread* Thread::runningThread = nullptr;

Thread::Thread()
{
    //myStack = new char[DEFAULT_STACK_SIZE];
    //systemStack = new char[SYSTEM_STACK_SIZE];
}

Thread::~Thread()
{
    //delete[] myStack;
    //delete[] systemStack;
}
