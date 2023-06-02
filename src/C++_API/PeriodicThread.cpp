#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/TCB.hpp"

void PeriodicThread::terminate()
{
    work = false;
}

PeriodicThread::PeriodicThread(time_t period)
    :
    period(period),
    work(true)
{
}

void PeriodicThread::run()
{
    while(work)
    {
        periodicActivation();
        time_sleep(period);
    }
}