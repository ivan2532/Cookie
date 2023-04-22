#ifndef _CCB_hpp_
#define _CCB_hpp_

#include "../../lib/hw.h"
#include "Scheduler.hpp"

class CCB
{
public:
    using Body = void(*)();

    static CCB* createCoroutine(Body body);
    static void yield();

    static CCB* running;

    bool isFinished() const { return m_Finished; }
    void setFinished(bool finished) { m_Finished = finished; }

    ~CCB() { delete[] m_Stack; }

private:
    CCB(Body body)
        :
        m_Body(body),
        m_Stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
        m_Finished(false)
    {
        // When creating an initial context, we want ra to point to the body of
        // our coroutine immediately, and sp will point at the start of the space
        // allocated for the stack
        m_Context =
        {
            m_Body != nullptr ? (uint64)body : 0,
            m_Stack != nullptr ? (uint64)&m_Stack[STACK_SIZE] : 0
        };

        if(body != nullptr)
        {
            Scheduler::put(this);
        }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body m_Body;
    uint64* m_Stack;
    Context m_Context;
    bool m_Finished;

    // Switch coroutine context (ra and sp)
    static void contextSwitch(Context* oldContext, Context* newContext);
    static void dispatch();

    static constexpr uint64 STACK_SIZE = 1024;
};


#endif //_CCB_hpp_
