#ifndef _TCB_hpp_
#define _TCB_hpp_

#include "../../lib/hw.h"
#include "Scheduler.hpp"

class TCB
{
    friend class Kernel;
    friend class SCB;
    friend void PeriodicThread::terminate();

public:
    using Body = void(*)(void*);

    static TCB* createThread(Body body, void* args, void* stack, bool kernelThread = false);

    static KernelDeque<TCB*> allThreads;
    static KernelDeque<TCB*> suspendedThreads;
    static TCB* running;

    void waitForThread(TCB* handle);
    void unblockWaitingThread();

    static int sleep(uint64);

    ~TCB();

private:
    TCB(Body body, void* args, uint64 timeSlice, void* stack, bool kernelThread = false);

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body m_Body;
    void* m_Args;
    void* m_Stack;
    Context m_Context;
    uint64 m_TimeSlice;
    bool m_Finished;
    KernelDeque<TCB*> m_WaitingThreads;
    uint64 m_SleepCounter;
    bool m_PutInScheduler;
    bool m_KernelThread;

    static TCB* mainThread;
    static TCB* idleThread;
    static TCB* outputThread;
    static TCB* userThread;

    [[noreturn]] static void idleThreadBody(void*);
    [[noreturn]] static void outputThreadBody(void*);

    static void bodyWrapper();

    static void contextSwitch(Context* oldContext, Context* newContext);

    static void dispatch();
    static int deleteThread(TCB* handle);

    static uint64 timeSliceCounter;
};


#endif //_TCB_hpp_
