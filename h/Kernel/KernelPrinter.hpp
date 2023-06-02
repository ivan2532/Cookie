#ifndef _Kernel_Printer_hpp_
#define _Kernel_Printer_hpp_

#include "../C++_API/syscall_cpp.hpp"

class KernelPrinter
{
public:
    static void printString(char const *string);
    static void printInteger(uint64 integer);
};

#endif // _Kernel_Printer_hpp_