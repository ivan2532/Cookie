#include "../../h/C++_API/syscall_cpp.hpp"
#include "../../h/Kernel/CCB.hpp"
#include "../../h/Kernel/workers.hpp"
#include "../../h/Kernel/print.hpp"

extern "C" void setTrap();

int main()
{
    setTrap();

    CCB* coroutines[3];

    // Create main coroutine, and set it to running
    // When we create a main couroutine (specific case when body = nullptr,
    // we don't put it in the Scheduler, it will gain it's returning Context
    // once it gives the processor to another coroutine
    coroutines[0] = CCB::createCoroutine(nullptr);
    CCB::running = coroutines[0];

    // Create two worker coroutines, createCoroutine will add them to the Scheduler
    coroutines[1] = CCB::createCoroutine(workerBodyA);
    printString("CoroutineA created\n");
    coroutines[2] = CCB::createCoroutine(workerBodyB);
    printString("CoroutineB created\n");

    // Make main give the processor to some other coroutine until all other coroutines
    // are done.
    while(!(coroutines[1]->isFinished() && coroutines[2]->isFinished()))
    {
        CCB::yield();
    }

    for(auto& coroutine : coroutines) delete coroutine;

    return 0;
}