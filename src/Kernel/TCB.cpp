#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/kernel_allocator.h"
#include "../../h/Kernel/print.hpp"

List<TCB> TCB::allThreads;
List<TCB> TCB::suspendedThreads;
TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;

void TCB::bodyWrapper()
{
    // The thread is created and should start from here, we are still in the supervisor regime
    // Continue program execution from here and return from the trap
    Riscv::returnFromSystemCall();

    running->m_Body(running->m_Args);
    running->m_Finished = true;

    // Once the thread has finished execution, exit
    thread_exit();
}

void TCB::getNewRunning()
{
    // Get first ready thread that is not suspended
    do running = Scheduler::get(true);
    while(suspendedThreads.contains(running));
}

void TCB::dispatch(bool putOldThreadInScheduler)
{
    auto old = running;

    // We don't want to put suspended threads into the Scheduler
    if(putOldThreadInScheduler && !old->m_Finished)
    {
        Scheduler::put(old,false,true);
    }
    getNewRunning();

    TCB::contextSwitch(&old->m_Context, &running->m_Context, &Riscv::kernelLock);
}

int TCB::deleteThread(TCB* handle)
{
    if(handle == nullptr) return -1;

    // Handle deletion of main thread
    if(handle->m_Body == nullptr)
    {
        delete handle;
        return 0;
    }

    // Unblock thread that is waiting for this thread to finish
    handle->unblockWaitingThread();

    auto handleIsRunning = (running == handle);
    delete handle;

    // If we are deleting the running thread, change context and don't save old context
    if(handleIsRunning)
    {
        getNewRunning();
        TCB::contextSwitch(nullptr, &running->m_Context, &Riscv::kernelLock);
    }

    return 0;
}

TCB* TCB::createThread(TCB::Body body, void* args, void* stack)
{
    auto newTCB = static_cast<TCB*>(kernel_alloc(sizeof(TCB)));
    new (newTCB) TCB(body, args, DEFAULT_TIME_SLICE, (uint64*)stack, true);

    // If we are creating the main thread, set it as running
    // All other threads go to TCB::allThreads
    if(body == nullptr) running = newTCB;
    else allThreads.addLast(newTCB);

    return newTCB;
}

void TCB::waitForThread(TCB* handle)
{
    // Can't wait for current thread, check if thread exists
    if(handle == this || !allThreads.contains(handle)) return;

    // Add thread to suspended queue
    suspendedThreads.addLast(this);

    // Add waiting thread that we want to unblock later
    handle->m_waitingThreads.addLast(this);

    // Change context and don't put suspended thread into the Scheduler
    Riscv::contextSwitch(false);
}

void TCB::unblockWaitingThread()
{
    // If no thread is waiting, return
    if(m_waitingThreads.isEmpty()) return;

    // Unblock waiting threads
    while(auto waitingThread = m_waitingThreads.removeFirst())
    {
        TCB::suspendedThreads.remove(waitingThread);
        Scheduler::put(waitingThread);
    }
}
