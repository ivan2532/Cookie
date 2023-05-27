#include "../../h/C++_API/syscall_cpp.hpp"

Semaphore::Semaphore(unsigned int init)
    :
    myHandle(nullptr)
{
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore()
{
    sem_close(myHandle);
}

int Semaphore::wait()
{
    return sem_wait(myHandle);
}

int Semaphore::signal()
{
    return sem_signal(myHandle);
}
