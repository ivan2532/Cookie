#ifndef _SCB_hpp_
#define _SCB_hpp_

#include "TCB.hpp"
#include "KernelDeque.hpp"

class SCB
{
public:
    explicit SCB(unsigned startValue = 1, bool binary = false);
    ~SCB();

    void wait();
    void signal();

protected:
    void block();
    void unblock();

    int m_Value;
    bool m_Binary;

private:
    KernelDeque<TCB*> m_BlockedQueue;
};

#endif //_SCB_hpp_
