#include "../../h/Kernel/Scheduler.hpp"

List<TCB> Scheduler::threadQueue;

TCB *Scheduler::get(bool useKernelAllocator)
{
    return threadQueue.removeFirst(useKernelAllocator);
}

void Scheduler::put(TCB* handle, bool putAtFrontOfQueue, bool useKernelAllocator)
{
    if(putAtFrontOfQueue) {
        threadQueue.addFirst(handle, useKernelAllocator);
    }
    else {
        threadQueue.addLast(handle, useKernelAllocator);
    }
}

bool Scheduler::contains(TCB *handle)
{
    return threadQueue.contains(handle);
}
