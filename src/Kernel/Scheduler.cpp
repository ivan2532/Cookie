#include "../../h/Kernel/Scheduler.hpp"

List<TCB> Scheduler::threadQueue;

TCB *Scheduler::get()
{
    return threadQueue.removeFirst(true);
}

void Scheduler::put(TCB* handle, bool putAtFrontOfQueue)
{
    if(putAtFrontOfQueue) {
        threadQueue.addFirst(handle, true);
    }
    else {
        threadQueue.addLast(handle, true);
    }
}

bool Scheduler::contains(TCB *handle)
{
    return threadQueue.contains(handle);
}
