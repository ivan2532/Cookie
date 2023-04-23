#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/workers.hpp"
#include "../../h/Kernel/print.hpp"
#include "../../h/Kernel/Riscv.hpp"

int main()
{
    // Enable interrupts
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    // Set our trap handler
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB* threads[5];

    // Create main thread, and set it to running
    // When we create a main thread (specific case when body = nullptr,
    // we don't put it in the Scheduler, it will gain it's returning Context
    // once it gives the processor to another thread
    threads[0] = TCB::createThread(nullptr);
    TCB::running = threads[0];

    // Create worker threads, createThread will add them to the Scheduler
    threads[1] = TCB::createThread(workerBodyA);
    printString("ThreadA created\n");
    threads[2] = TCB::createThread(workerBodyB);
    printString("ThreadB created\n");
    threads[3] = TCB::createThread(workerBodyC);
    printString("ThreadC created\n");
    threads[4] = TCB::createThread(workerBodyD);
    printString("ThreadD created\n");

    // Make main thread give the processor to someone else until all other threads
    // are done.
    while(true)
    {
        auto allWorkerThreadsFinished = true;
        for(auto thread : threads)
        {
            if(thread != threads[0] && !thread->isFinished())
            {
                allWorkerThreadsFinished = false;
                break;
            }
        }
        if(allWorkerThreadsFinished) break;

        TCB::yield();
    }

    for(auto& thread : threads) delete thread;
    printString("Finished\n");

    return 0;
}