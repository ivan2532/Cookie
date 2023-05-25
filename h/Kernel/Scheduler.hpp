#ifndef _Scheduler_hpp_
#define _Scheduler_hpp_

#include "List.hpp"

class TCB;

class Scheduler
{
private:
    static List<TCB> threadQueue;

public:
    static TCB *get(bool useKernelAllocator = false);
    static void put(TCB *handle, bool putAtFrontOfQueue = false, bool useKernelAllocator = false);
    static bool contains(TCB *handle);
};

#endif //_Scheduler_hpp_
