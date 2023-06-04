#include "../../h/Kernel/KernelPrinter.hpp"
#include "../../h/Kernel/Kernel.hpp"
#include "../../h/Kernel/SCB.hpp"

constexpr char KernelPrinter::digits[];

void KernelPrinter::printString(char const *string)
{
    Kernel::lock();

    while (*string != '\0')
    {
        Kernel::addCharToOutputBuffer(*string);
        string++;
    }

    Kernel::unlock();
}

void KernelPrinter::printNumber(uint64 integer, uint8 base)
{
    Kernel::lock();

    char buf[64];
    auto i = 0;

    do buf[i++] = digits[integer % base];
    while((integer /= base) != 0);

    if(base == 2)
    {
        Kernel::addCharToOutputBuffer('0');
        Kernel::addCharToOutputBuffer('b');
    }
    else if(base == 16)
    {
        Kernel::addCharToOutputBuffer('0');
        Kernel::addCharToOutputBuffer('x');
    }

    while(--i >= 0) Kernel::addCharToOutputBuffer(buf[i]);

    Kernel::unlock();
}
