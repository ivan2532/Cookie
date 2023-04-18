#include "../../lib/console.h"
#include "../../h/C++_API/syscall_cpp.hpp"

#include "../../h/Kernel/Queue.hpp"

extern "C" void setTrap();

int main()
{
    setTrap();

    Queue<char> testQueue;
    testQueue.enqueue('a');
    testQueue.enqueue('b');
    testQueue.enqueue('c');

    while(testQueue.size() > 0)
    {
        __putc(testQueue.dequeue());
    }

    return 0;
}