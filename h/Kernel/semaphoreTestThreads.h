#include "../C++_API/syscall_cpp.hpp"
#include "print.hpp"

class ThreadA : public Thread
{
public:
    ThreadA(sem_t semaphoreA, sem_t semaphoreB)
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
            printString("A Counter: ");
            printInteger(m_CounterA);
            printString("\n");

            sem_signal(m_SemaphoreA);
            if(m_CounterA == 2147483640) break;

            sem_wait(m_SemaphoreB);
        }
    }

private:
    sem_t m_SemaphoreA;
    sem_t m_SemaphoreB;
    int m_CounterA;
};

class ThreadB : public Thread
{
public:
    ThreadB(sem_t semaphoreA, sem_t semaphoreB)
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
            sem_wait(m_SemaphoreA);

            m_CounterB++;
            printString("B Counter: ");
            printInteger(m_CounterB);
            printString("\n");

            sem_signal(m_SemaphoreB);
            if(m_CounterB == 2147483640) break;
        }
    }

private:
    sem_t m_SemaphoreA;
    sem_t m_SemaphoreB;
    int m_CounterB;
};
