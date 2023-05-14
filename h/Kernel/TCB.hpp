#ifndef _TCB_hpp_
#define _TCB_hpp_

#include "../../lib/hw.h"
#include "Scheduler.hpp"

class TCB
{
    friend class Riscv;

public:
    using Body = void(*)(void*);

    static TCB* createThread(Body body, void* args, void* stack);

    static List<TCB> allThreads;
    static TCB* running;

    bool isFinished() const { return m_Finished; }

    ~TCB()
    {
        allThreads.remove(this);
        delete[] m_Stack;
    }

private:
    // When creating an initial context, we want ra to point to the body of
    // our thread immediately, and sp will point at the start of the space
    // allocated for the stack
    TCB(Body body, void* args, uint64 timeSlice, uint64* stack)
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
        m_Finished(false)
    {
        if(body != nullptr) Scheduler::put(this);
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

    static void bodyWrapper();

    // Switch coroutine context (ra and sp)
    static void contextSwitch(Context* oldContext, Context* newContext);
    static void dispatch();
    static int deleteRunningThread();

    static uint64 timeSliceCounter;
};


#endif //_TCB_hpp_
