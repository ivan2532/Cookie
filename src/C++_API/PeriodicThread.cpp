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
    start();
}

[[noreturn]] void PeriodicThread::run()
{
    while(true)
    {
        time_sleep(period);
        periodicActivation();
    }
}