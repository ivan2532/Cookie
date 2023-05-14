#ifndef _Scheduler_hpp_
#define _Scheduler_hpp_

#include "List.hpp"

class TCB;

class Scheduler
{
private:
    static List<TCB> threadQueue;

public:
    static TCB *get();
    static void put(TCB *ccb);
};

#endif //_Scheduler_hpp_
