#ifndef _syscall_cpp
#define _syscall_cpp

#include "../C_API/syscall_c.h"

typedef unsigned int  thread_t;

void* operator new (size_t size);
void operator delete (void* ptr);

struct ContextBuffer
{
    uint64 registers[32];
};

class Thread
{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);
    static Thread* GetRunningThread() { return runningThread; }
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    char* myStack;
    ContextBuffer myContext;

    static Thread* runningThread;
};

#endif
