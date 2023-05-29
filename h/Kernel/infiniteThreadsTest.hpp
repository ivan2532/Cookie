#ifndef _infinite_threads_test_hpp_
#define _infinite_threads_test_hpp_

#include "../C++_API/syscall_cpp.hpp"
#include "Riscv.hpp"
#include "../Tests/printing.hpp"

class InfiniteThreadA : public Thread
{
public:
    InfiniteThreadA()
    {

    }

    void run() override
    {
        while(true)
        {
            m_CounterA++;
            printString("Counter A: ");
            printInt(m_CounterA);
            printString("\n");
        }
    }

private:
    uint64 m_CounterA = 0;
};

class InfiniteThreadB : public Thread
{
public:
    InfiniteThreadB() = default;

    void run() override
    {
        while(true)
        {
            m_CounterB++;
            printString("Counter B: ");
            printInt(m_CounterB);
            printString("\n");
        }
    }

private:
    uint64 m_CounterB = 0;
};

class InfiniteThreadC : public Thread
{
public:
    InfiniteThreadC() = default;

    void run() override
    {
        while(true)
        {
            m_CounterC++;
            printString("Counter C: ");
            printInt(m_CounterC);
            printString("\n");
        }
    }

private:
    uint64 m_CounterC = 0;
};

#endif // _infinite_threads_test_hpp_
