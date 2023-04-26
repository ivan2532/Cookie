#include "../../h/Kernel/print.hpp"
#include "../../lib/console.h"
#include "../../h/Kernel/Riscv.hpp"

void printString(char const *string)
{
    //auto sstatus = Riscv::readSstatus();
    //Riscv::maskClearSstatus(Riscv::SSTATUS_SIE);

    while (*string != '\0')
    {
        __putc(*string);
        string++;
    }

    //Riscv::maskSetSstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

void printInteger(uint64 integer)
{
    //auto sstatus = Riscv::readSstatus();
    //Riscv::maskClearSstatus(Riscv::SSTATUS_SIE);

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

    while (--i >= 0)
        __putc(buf[i]);

    //Riscv::maskSetSstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}