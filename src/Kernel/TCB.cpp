#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/SCB.hpp"

KernelDeque<TCB*> TCB::allThreads;
KernelDeque<TCB*> TCB::suspendedThreads;
TCB* TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB* TCB::mainThread = nullptr;
TCB* TCB::idleThread = nullptr;
TCB* TCB::outputThread = nullptr;
TCB* TCB::userThread = nullptr;

void TCB::bodyWrapper()
{
    // The thread is created and should start from here, we are still in the supervisor regime
    // Continue program execution from here and return from the trap

    if(!TCB::running->m_KernelThread) Riscv::returnFromSystemCall();
    else Riscv::unlock();

    running->m_Body(running->m_Args);
    running->m_Finished = true;

    // Once the thread has finished execution, exit
    if(running->m_KernelThread) deleteThread(running);
    else thread_exit();
}

void TCB::dispatch()
{
    auto old = running;

    // We don't want to put suspended threads into the Scheduler
    if(running->m_PutInScheduler && !old->m_Finished) Scheduler::put(old);
    else running->m_PutInScheduler = true;
    running = Scheduler::get();

    if(running != old) TCB::contextSwitch(&old->m_Context, &running->m_Context);
}

int TCB::deleteThread(TCB* handle)
{
    if(handle == nullptr) return -1;

    // Unblock thread that is waiting for this thread to finish
    handle->unblockWaitingThread();

    auto handleIsRunning = (running == handle);
    MemoryAllocator::free(handle);

    // If we are deleting the running thread, change context and don't save old context
    if(handleIsRunning) thread_dispatch();

    return 0;
}

TCB* TCB::createThread(TCB::Body body, void* args, void* stack, bool kernelThread)
{
    auto newTCB = static_cast<TCB*>(MemoryAllocator::alloc(sizeof(TCB)));
    new (newTCB) TCB
    (
        body,
        args,
        DEFAULT_TIME_SLICE,
        stack,
        kernelThread
    );

    // If we are creating the main thread, set it as running
    if(body == nullptr) running = newTCB;

    allThreads.addLast(newTCB);
    return newTCB;
}

void TCB::waitForThread(TCB* handle)
{
    // Can't wait for current thread, check if thread exists
    if(handle == this || !allThreads.contains(handle)) return;

    // Add thread to suspended queue
    suspendedThreads.addLast(this);

    // Add waiting thread that we want to unblock later
    handle->m_WaitingThreads.addLast(this);

    // Change context
    running->m_PutInScheduler = false;
    thread_dispatch();
}

void TCB::unblockWaitingThread()
{
    // If no thread is waiting, return
    if(m_WaitingThreads.isEmpty()) return;

    // Unblock waiting threads
    while(!m_WaitingThreads.isEmpty())
    {
        auto waitingThread = m_WaitingThreads.removeFirst();
        TCB::suspendedThreads.remove(waitingThread);
        Scheduler::put(waitingThread);
    }
}

int TCB::sleep(uint64 time)
{
    // If thread is already asleep, return -1
    if(TCB::running->m_SleepCounter > 0) return -1;
    if(time == 0) return 0;

    TCB::running->m_SleepCounter = time;
    TCB::running->m_PutInScheduler = false;
    thread_dispatch();
    return 0;
}

[[noreturn]] void TCB::idleThreadBody(void*)
{
    while(true) thread_dispatch();
}

[[noreturn]] void TCB::outputThreadBody(void*)
{
    while(true)
    {
        Riscv::lock();

        Riscv::outputSemaphore->wait();
        Riscv::outputControllerReadySemaphore->wait();

        auto pOutData = (char*)CONSOLE_TX_DATA;
        *pOutData = Riscv::outputQueue.removeFirst();

        Riscv::unlock();
    }
}
