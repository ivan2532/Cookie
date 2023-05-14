#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/workers.hpp"
#include "../../h/Kernel/print.hpp"
#include "../../h/Kernel/Riscv.hpp"

int main()
{
    // Enable interrupts
    Riscv::maskSetSstatus(Riscv::SSTATUS_SIE);

    // Set our trap handler, save the old one so we can restore it after our kernel has finished
    auto oldTrap = Riscv::readStvec();
    Riscv::writeStvec((uint64) &Riscv::supervisorTrap);

    thread_t threads[5];

    // Create main thread, and set it to running
    // When we create a main thread (specific case when body = nullptr,
    // we don't put it in the Scheduler, it will gain it's returning Context
    // once it gives the processor to another thread
    thread_create(&threads[0], nullptr, nullptr);

    // Create worker threads, createThread will add them to the Scheduler
    thread_create(&threads[1], workerBodyA, nullptr);
    printString("ThreadA created\n");
    thread_create(&threads[2], workerBodyB, nullptr);
    printString("ThreadB created\n");
    thread_create(&threads[3], workerBodyC, nullptr);
    printString("ThreadC created\n");
    thread_create(&threads[4], workerBodyD, nullptr);
    printString("ThreadD created\n");

    // Wait for all threads
    for(auto thread : threads) thread_join(thread);

//    // Make main thread give the processor to someone else until all other threads
//    // are done.
//    while(true)
//    {
//        auto allWorkerThreadsFinished = true;
//        for(auto thread : threads)
//        {
//            if(thread != threads[0] && thread != threads[1] && !thread->isFinished())
//            {
//                allWorkerThreadsFinished = false;
//                break;
//            }
//        }
//        if(allWorkerThreadsFinished) break;
//
//        thread_dispatch();
//    }

    for(auto& thread : threads) delete thread;

    // We are done, restore the old trap
    Riscv::writeStvec(oldTrap);

    printString("Finished\n");
    return 0;
}