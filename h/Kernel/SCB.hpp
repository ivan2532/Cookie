#ifndef _SCB_hpp_
#define _SCB_hpp_

#include "TCB.hpp"
#include "List.hpp"

class SCB
{
public:
    explicit SCB(unsigned startValue = 1)
        :
        value((int)startValue)
    {
    }
    ~SCB();

    void wait();
    void signal();

protected:
    void block();
    void unblock();

    int value;

private:
    List<TCB> blockedQueue;
};

#endif //_SCB_hpp_
