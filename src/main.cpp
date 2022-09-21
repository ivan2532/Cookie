#include "../lib/console.h"
#include "C++_API/syscall_cpp.h"

#include "Kernel/Queue.h"

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