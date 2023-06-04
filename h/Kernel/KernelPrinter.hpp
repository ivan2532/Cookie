#ifndef _Kernel_Printer_hpp_
#define _Kernel_Printer_hpp_

#include "../C++_API/syscall_cpp.hpp"

class KernelPrinter
{
public:
    static void printString(char const *string);
    static void printNumber(uint64 integer, uint8 base = 10);

private:
    static constexpr char digits[] = "0123456789ABCDEF";
};

#endif // _Kernel_Printer_hpp_