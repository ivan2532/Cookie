#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/workers.hpp"
#include "../../h/Tests//printing.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/semaphoreTestThreads.h"
#include "../../h/Kernel/infiniteThreadsTest.hpp"
#include "../../h/Kernel/periodicTestThread.hpp"
#include "../../lib/console.h"

extern void userMain();

void workersTest()
{
    // Create and start worker threads, createThread will add them to the Scheduler
    Thread workerA(workerBodyA, nullptr);
    printString("WorkerA created\n");

    Thread workerB(workerBodyB, nullptr);
    printString("WorkerB created\n");

    Thread workerC(workerBodyC, nullptr);
    printString("WorkerC created\n");

    Thread workerD(workerBodyD, nullptr);
    printString("WorkerD created\n");

    // Wait for all threads
    workerA.join();
    workerB.join();
    workerC.join();
    workerD.join();
}

void infiniteThreadsTest()
{
    InfiniteThreadA infiniteThreadA;
    InfiniteThreadB infiniteThreadB;
    InfiniteThreadC infiniteThreadC;

    infiniteThreadA.start();
    infiniteThreadB.start();
    infiniteThreadC.start();

    infiniteThreadA.join();
    infiniteThreadB.join();
    infiniteThreadC.join();
}

void semaphoreTest()
{
    Semaphore semaphoreA(0);
    Semaphore semaphoreB(0);

    ThreadA threadA(semaphoreA, semaphoreB);
    ThreadB threadB(semaphoreA, semaphoreB);

    threadB.start();
    threadA.start();

    threadA.join();
    threadB.join();
}

void periodicThreadTest()
{
    PeriodicTestThread thread;
    thread.start();
    thread.join();
}

void userMainWrapper(void*) { userMain(); }

int main()
{
    // Set our trap handler, save the old one so we can restore it after our kernel has finished
    auto oldTrap = Riscv::readStvec();
    Riscv::writeStvec((uint64) &Riscv::supervisorTrap + 1);

    // Enable interrupts
    Riscv::maskSetSstatus(Riscv::SSTATUS_SIE);

    // Create main thread
    // When we create a main thread (specific case when body = nullptr) we don't put it in the Scheduler,
    // it will gain it's returning Context once it gives the processor to another thread
    thread_t mainThread;
    thread_create(&mainThread, nullptr, nullptr);
    printString("Main thread created\n");

    thread_create(&TCB::idleThread, &TCB::idleThreadBody, nullptr);
    TCB::idleThread->m_TimeSlice = 0;

    // Create user thread
    thread_t userThread;
    thread_create(&userThread, userMainWrapper, nullptr);
    thread_join(userThread);

    // Delete main thread
    delete mainThread;
    printString("Finished\n");

    // We are done, restore the old trap
    Riscv::writeStvec(oldTrap);

    return 0;
}