#ifndef _PERIODIC_TEST_THREAD_H_
#define _PERIODIC_TEST_THREAD_H_

#include "../C++_API/syscall_cpp.hpp"
#include "print.hpp"

class PeriodicTestThread : public Thread
{
public:
    PeriodicTestThread()
        :
        m_Counter(0)
    {
    }

    void run() override
    {
        while(true)
        {
            m_Counter++;
            printString("Counter: ");
            printInteger(m_Counter);
            printString("\n");
            Thread::sleep(10);
        }
    }

private:
    int m_Counter;
};

#endif // _PERIODIC_TEST_THREAD_H_
