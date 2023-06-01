#ifndef _Scheduler_hpp_
#define _Scheduler_hpp_

#include "KernelList.hpp"

class TCB;

class Scheduler
{
private:
    static KernelList<TCB*> threadQueue;

public:
    static TCB *get();
    static void put(TCB *handle, bool putAtFrontOfQueue = false);
    static bool contains(TCB *handle);
};

#endif //_Scheduler_hpp_
