#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/C_API/syscall_c.hpp"

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
    do running = Scheduler::get();
    while(suspendedThreads.contains(running));
}

void TCB::dispatch(bool putOldThreadInScheduler)
{
    auto old = running;

    if(putOldThreadInScheduler && !old->m_Finished) Scheduler::put(old);
    getNewRunning();

    TCB::contextSwitch(&old->m_Context, &running->m_Context);
}

int TCB::deleteThread(TCB* handle)
{
    if(handle == nullptr || handle->m_Body == nullptr) return -1;

    handle->unblockWaitingThread();
    auto handleIsRunning = (running == handle);
    delete handle;

    if(handleIsRunning)
    {
        getNewRunning();
        TCB::contextSwitch(nullptr, &running->m_Context);
    }

    return 0;
}

TCB *TCB::createThread(TCB::Body body, void* args, void* stack)
{
    auto result = new TCB(body, args, DEFAULT_TIME_SLICE, (uint64*)stack);

    if(body == nullptr) running = result;
    else allThreads.addLast(result);

    return result;
}

void TCB::waitForThread(TCB* handle)
{
    if(handle == this || !allThreads.contains(handle)) return;

    suspendedThreads.addLast(this);
    handle->m_waitingThread = this;
    dispatch(false);
}

void TCB::unblockWaitingThread()
{
    if(m_waitingThread == nullptr) return;

    TCB::suspendedThreads.remove(m_waitingThread);
    Scheduler::put(m_waitingThread);
    m_waitingThread = nullptr;
}
