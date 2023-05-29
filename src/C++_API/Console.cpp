#include "../../h/C++_API/syscall_cpp.hpp"

char Console::getc()
{
    return ::getc();
}

void Console::putc(char output)
{
    ::putc(output);
}
