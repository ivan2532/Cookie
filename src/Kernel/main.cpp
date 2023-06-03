#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/SCB.hpp"
#include "../../h/Tests//printing.hpp"
#include "../../h/Kernel/Riscv.hpp"

extern void userMain();

void userMainWrapper(void*) { userMain(); }

inline void startSystemThreads()
{
    // Create main thread
    // When we create the main thread (specific case when body = nullptr) we don't put it in the Scheduler,
    // it will gain it's returning context once it gives the processor to another thread
    TCB::mainThread = TCB::createThread(nullptr, nullptr, nullptr, true);

    // Create idle thread
    auto idleThreadStack = MemoryAllocator::alloc(DEFAULT_STACK_SIZE + STACK_CONTEXT_EXTENSION);
    TCB::idleThread = TCB::createThread
    (
        TCB::idleThreadBody,
        nullptr,
        idleThreadStack,
        true
    );
}

inline void startIO()
{
    // Create io semaphores
    Riscv::inputSemaphore = static_cast<SCB*>(MemoryAllocator::alloc(sizeof(SCB)));
    Riscv::outputSemaphore = static_cast<SCB*>(MemoryAllocator::alloc(sizeof(SCB)));
    Riscv::outputControllerReadySemaphore = static_cast<SCB*>(MemoryAllocator::alloc(sizeof(SCB)));
    new (Riscv::inputSemaphore) volatile SCB(0);
    new (Riscv::outputSemaphore) volatile SCB(0);
    new (Riscv::outputControllerReadySemaphore) volatile SCB(0);

    // Create io thread
    auto outputThreadStack = MemoryAllocator::alloc(DEFAULT_STACK_SIZE + STACK_CONTEXT_EXTENSION);
    TCB::outputThread = TCB::createThread
    (
        TCB::outputThreadBody,
        nullptr,
        outputThreadStack,
        true
    );

    // Enable interrupts
    Riscv::maskSetSstatus(Riscv::SSTATUS_SIE);
    thread_dispatch();
}

inline void startUserThread()
{
    // Create user thread
    auto userThreadStack = MemoryAllocator::alloc(DEFAULT_STACK_SIZE + STACK_CONTEXT_EXTENSION);
    TCB::userThread = TCB::createThread(userMainWrapper, nullptr, userThreadStack);
    thread_dispatch();

    // Wait for user thread to finish
    TCB::running->waitForThread(TCB::userThread);
}

int main()
{
    // Set our trap handler, save the old one so we can restore it after our kernel has finished
    auto oldTrap = Riscv::readStvec();
    Riscv::writeStvec((uint64) &Riscv::supervisorTrap + 1);

    startSystemThreads();
    startIO();
    startUserThread();

    // We are done, restore the old trap and delete system threads
    delete TCB::mainThread;
    delete TCB::idleThread;
    delete TCB::outputThread;
    Riscv::writeStvec(oldTrap);

    return 0;
}