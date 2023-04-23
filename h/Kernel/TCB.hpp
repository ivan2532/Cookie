#ifndef _TCB_hpp_
#define _TCB_hpp_

#include "../../lib/hw.h"
#include "Scheduler.hpp"

class TCB
{
    friend class Riscv;

public:
    using Body = void(*)();

    static TCB* createThread(Body body);
    static void yield();

    static TCB* running;

    bool isFinished() const { return m_Finished; }

    ~TCB() { delete[] m_Stack; }

private:
    // When creating an initial context, we want ra to point to the body of
    // our thread immediately, and sp will point at the start of the space
    // allocated for the stack
    TCB(Body body, uint64 timeSlice)
        :
        m_Body(body),
        m_Stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
        m_Context
        ({
            (uint64)&bodyWrapper,
            m_Stack != nullptr ? (uint64)&m_Stack[STACK_SIZE] : 0
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
    uint64* m_Stack;
    Context m_Context;
    uint64 m_timeSlice;
    bool m_Finished;

    static void bodyWrapper();

    // Switch coroutine context (ra and sp)
    static void contextSwitch(Context* oldContext, Context* newContext);
    static void dispatch();

    static uint64 timeSliceCounter;

    static constexpr uint64 STACK_SIZE = 1024;
    static constexpr uint64 DEFAULT_TIME_SLICE = 2;
};


#endif //_TCB_hpp_
