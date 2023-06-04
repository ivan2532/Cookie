#include "../../h/Kernel/KernelPrinter.hpp"
#include "../../h/Kernel/Kernel.hpp"
#include "../../h/Kernel/SCB.hpp"

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

void KernelPrinter::printInteger(uint64 integer)
{
    static char digits[] = "0123456789";
    char buf[16];
    int i;
    uint x;

    x = integer;

    i = 0;
    do
    {
        buf[i++] = digits[x % 10];
    } while ((x /= 10) != 0);

    while (--i >= 0) { Kernel::addCharToOutputBuffer(buf[i]); }
}
