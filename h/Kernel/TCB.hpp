#ifndef _TCB_hpp_
#define _TCB_hpp_

#include "../../lib/hw.h"
#include "Scheduler.hpp"

class TCB
{
    friend class Riscv;
    friend class SCB;

public:
    using Body = void(*)(void*);

    static TCB* createThread(Body body, void* args, void* stack);

    static List<TCB> allThreads;
    static List<TCB> suspendedThreads;
    static TCB* running;

    bool isFinished() const { return m_Finished; }

    void waitForThread(TCB* handle);
    void unblockWaitingThread();

    ~TCB()
    {
        allThreads.remove(this);
        suspendedThreads.remove(this);
        delete[] m_Stack;
    }

private:
    // When creating an initial context, we want ra to point to the body of
    // our thread immediately, and sp will point at the start of the space
    // allocated for the stack
    TCB(Body body, void* args, uint64 timeSlice, uint64* stack, bool putAtFrontOfSchedulerQueue = false)
        :
            m_Body(body),
            m_Args(args),
            m_Stack(body != nullptr ? stack : nullptr),
            m_Context
        ({
            (uint64)&bodyWrapper,
            m_Stack != nullptr ? (uint64)&m_Stack[DEFAULT_STACK_SIZE] : 0
         }),
            m_timeSlice(timeSlice),
            m_Finished(false),
            m_waitingThread(nullptr)
    {
        if(body != nullptr) Scheduler::put(this, putAtFrontOfSchedulerQueue);
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body m_Body;
    void* m_Args;
    uint64* m_Stack;
    Context m_Context;
    uint64 m_timeSlice;
    bool m_Finished;
    TCB* m_waitingThread;

    static void bodyWrapper();

    static void contextSwitch(Context* oldContext, Context* newContext);
    static void getNewRunning();
    static void dispatch(bool putOldThreadInScheduler = true);
    static int deleteThread(TCB* handle);

    static uint64 timeSliceCounter;
};


#endif //_TCB_hpp_
