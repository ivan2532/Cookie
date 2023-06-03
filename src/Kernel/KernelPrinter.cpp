#include "../../h/Kernel/KernelPrinter.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/SCB.hpp"

void KernelPrinter::printString(char const *string)
{
    Riscv::lock();

    while (*string != '\0')
    {
        Riscv::addCharToOutputBuffer(*string);
        string++;
    }

    Riscv::unlock();
}

void KernelPrinter::printInteger(uint64 integer)
{
    Riscv::lock();

    static char digits[] = "0123456789";
    char buf[16];
    auto i = 0;
    auto x = integer;

    do buf[i++] = digits[x % 10];
    while ((x /= 10) != 0);

    while (--i >= 0) Riscv::addCharToOutputBuffer(buf[i]);

    Riscv::unlock();
}
