/*
    Basic FIFO scheduler
*/

#ifndef _Scheduler_hpp_
#define _Scheduler_hpp_

#include "Queue.hpp"

class Thread;

class Scheduler
{
public:
    static Scheduler* Instance()
    {
        if(m_Instance == nullptr)
        {
            m_Instance = new Scheduler();
        }

        return m_Instance;
    }

    void put(Thread* thread)
    {
        ready.enqueue(thread);
    }

    Thread* get()
    {
        return ready.dequeue();
    }

private:
    static Scheduler* m_Instance;
    Queue<Thread*> ready;

    ~Scheduler()
    {
        delete m_Instance;
    }
};

Scheduler* Scheduler::m_Instance = nullptr;

#endif //_Scheduler_hpp_
