#ifndef _Scheduler_hpp_
#define _Scheduler_hpp_

#include "List.hpp"

class CCB;

class Scheduler
{
private:
    static List<CCB> readyCoroutineQueue;

public:
    static CCB *get();

    static void put(CCB *ccb);

};

#endif //_Scheduler_hpp_
