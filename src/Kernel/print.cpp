#include "../../h/Kernel/print.hpp"
#include "../../lib/console.h"

void printString(char const *string)
{
    while (*string != '\0')
    {
        __putc(*string);
        string++;
    }
}

void printInteger(uint64 integer)
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

    while (--i >= 0)
        __putc(buf[i]);
}