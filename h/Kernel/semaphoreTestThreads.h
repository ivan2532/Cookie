#include "../C++_API/syscall_cpp.hpp"
#include "print.hpp"

class ThreadA : public Thread
{
public:
    ThreadA(Semaphore& semaphoreA, Semaphore& semaphoreB)
        :
        m_SemaphoreA(semaphoreA),
        m_SemaphoreB(semaphoreB),
        m_CounterA(0)
    {
    }

    void run() override
    {
        while(true)
        {
            m_CounterA++;

            m_SemaphoreA.signal();
            if(m_CounterA == 2147483640) break;

            m_SemaphoreB.wait();
        }
    }

private:
    Semaphore& m_SemaphoreA;
    Semaphore& m_SemaphoreB;
    int m_CounterA;
};

class ThreadB : public Thread
{
public:
    ThreadB(Semaphore& semaphoreA, Semaphore& semaphoreB)
        :
        m_SemaphoreA(semaphoreA),
        m_SemaphoreB(semaphoreB),
        m_CounterB(0)
    {
    }

    void run() override
    {
        while(true)
        {
            m_SemaphoreA.wait();

            m_CounterB++;

            m_SemaphoreB.signal();
            if(m_CounterB == 2147483640) break;
        }
    }

private:
    Semaphore& m_SemaphoreA;
    Semaphore& m_SemaphoreB;
    int m_CounterB;
};
