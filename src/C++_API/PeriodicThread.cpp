#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/TCB.hpp"

void PeriodicThread::terminate()
{
    TCB::deleteThread(getMyHandle());
}

PeriodicThread::PeriodicThread(time_t period)
    :
    period(period)
{
}

[[noreturn]] void PeriodicThread::run()
{
    while(true)
    {
        periodicActivation();
        time_sleep(period);
    }
}