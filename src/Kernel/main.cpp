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
    auto idleThreadStack = kernel_alloc(DEFAULT_STACK_SIZE + STACK_CONTEXT_EXTENSION);
    TCB::idleThread = TCB::createThread
    (
        &TCB::idleThreadBody,
        nullptr,
        idleThreadStack,
        true
    );
    TCB::idleThread->m_TimeSlice = 0;
    TCB::allThreads.addLast(TCB::idleThread);
}

inline void startIO()
{
    // Create io semaphores
    Riscv::inputEmptySemaphore = static_cast<SCB*>(kernel_alloc(sizeof(SCB)));
    new (Riscv::inputEmptySemaphore) SCB(Riscv::InputBufferSize);

    Riscv::inputFullSemaphore = static_cast<SCB*>(kernel_alloc(sizeof(SCB)));
    new (Riscv::inputFullSemaphore) SCB(0);

    Riscv::outputEmptySemaphore = static_cast<SCB*>(kernel_alloc(sizeof(SCB)));
    new (Riscv::outputEmptySemaphore) SCB(Riscv::OutputBufferSize);

    Riscv::outputFullSemaphore = static_cast<SCB*>(kernel_alloc(sizeof(SCB)));
    new (Riscv::outputFullSemaphore) SCB(0);

    // Create io thread
    auto outputThreadStack = kernel_alloc(DEFAULT_STACK_SIZE + STACK_CONTEXT_EXTENSION);
    TCB::outputThread = TCB::createThread
    (
        &TCB::outputThreadBody,
        nullptr,
        outputThreadStack,
        true
    );
    TCB::allThreads.addLast(TCB::outputThread);
}

inline void startUserThread()
{
    // Create user thread
    auto userThreadStack = kernel_alloc(DEFAULT_STACK_SIZE + STACK_CONTEXT_EXTENSION);
    TCB::userThread = TCB::createThread(&userMainWrapper, nullptr, userThreadStack);
    TCB::allThreads.addLast(TCB::userThread);
    Riscv::contextSwitch();

    // Wait for user thread to finish
    TCB::running->waitForThread(TCB::userThread);
}

int main()
{
    // Set our trap handler, save the old one so we can restore it after our kernel has finished
    auto oldTrap = Riscv::readStvec();
    Riscv::writeStvec((uint64) &Riscv::supervisorTrap + 1);

    // Enable interrupts
    Riscv::maskSetSstatus(Riscv::SSTATUS_SIE);

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