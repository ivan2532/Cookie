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
            Riscv::lock();
            printString("Counter A: ");
            printInteger(m_CounterA);
            printString("\n");
            Riscv::unlock();

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
            Riscv::lock();
            printString("Counter B: ");
            printInteger(m_CounterB);
            printString("\n");
            Riscv::unlock();

            m_SemaphoreB.signal();
            if(m_CounterB == 2147483640) break;
        }
    }

private:
    Semaphore& m_SemaphoreA;
    Semaphore& m_SemaphoreB;
    int m_CounterB;
};
