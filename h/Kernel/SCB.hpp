#ifndef _SCB_hpp_
#define _SCB_hpp_

#include "TCB.hpp"
#include "List.hpp"

class SCB
{
public:
    explicit SCB(unsigned startValue = 1)
        :
            m_Value((int)startValue)
    {
    }
    ~SCB();

    void setValue(int value);

    void wait();
    void signal();

protected:
    void block();
    void unblock();

    int m_Value;

private:
    List<TCB> blockedQueue;
};

#endif //_SCB_hpp_
