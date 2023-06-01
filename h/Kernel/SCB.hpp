#ifndef _SCB_hpp_
#define _SCB_hpp_

#include "TCB.hpp"
#include "KernelDeque.hpp"

class SCB
{
public:
    explicit SCB(unsigned startValue = 1)
        :
        m_Value((int)startValue)
    {
    }
    ~SCB();

    void wait();
    void signal();

protected:
    void block();
    void unblock();

    int m_Value;

private:
    KernelDeque<TCB*> blockedQueue;
};

#endif //_SCB_hpp_
