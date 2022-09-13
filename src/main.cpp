#include "../lib/console.h"
#include "C++_API/syscall_cpp.h"

extern "C" void setTrap();

int main()
{
    setTrap();

    char* testAlloc = new char;
    *testAlloc = 'a';

    __putc(*testAlloc);

    delete testAlloc;

    return 0;
}