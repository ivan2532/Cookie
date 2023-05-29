#ifndef _PERIODIC_TEST_THREAD_H_
#define _PERIODIC_TEST_THREAD_H_

#include "../C++_API/syscall_cpp.hpp"
#include "../Tests/printing.hpp"

class PeriodicTestThread : public PeriodicThread
{
public:
    PeriodicTestThread()
        :
        PeriodicThread(10),
        m_Counter(0)
    {
    }

    void periodicActivation() override
    {
        m_Counter++;
        printString("Counter: ");
        printInt(m_Counter);
        printString("\n");
    }

private:
    int m_Counter;
};

#endif // _PERIODIC_TEST_THREAD_H_
