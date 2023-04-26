//
// RISC-V utility assembler functions
//

#ifndef _Riscv_hpp_
#define _Riscv_hpp_

#include "../../lib/hw.h"

class Riscv
{
public:

    // Pop sstatus.spp and sstatus.spie bits
    // (has to be a non inline function because we need ra)
    static void popSppSpie();

    // Read scause
    static uint64 readScause();

    // Write to scause
    static void writeScause(uint64 scause);

    // Read sepc
    static uint64 readSepc();

    // Write to sepc
    static void writeSepc(uint64 sepc);

    // Read stvec
    static uint64 readStvec();

    // Write to stvec
    static void writeStvec(uint64 stvec);

    // Read stval
    static uint64 readStval();

    // Write to stval
    static void writeStval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    // Mask set sip
    static void maskSetSip(uint64 mask);

    // Clears only the bits covered by the mask in sip register
    static void maskClearSip(uint64 mask);

    // Read sip
    static uint64 readSip();

    // Write sip
    static void writeSip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // Mask set sstatus
    static void maskSetSstatus(uint64 mask);

    // Mask clear sstatus
    static void maskClearSstatus(uint64 mask);

    // Read sstatus
    static uint64 readSstatus();

    // Write to sstatus
    static void writeSstatus(uint64 sstatus);

    static void supervisorTrap();

private:
    static void handleSupervisorTrap();
    inline static void handleSoftwareInterrupt();
    inline static void handleExternalInterrupt();
    inline static void handleEcall();
    inline static void handleUnknownTrapCause(uint64 scause);

    inline static void handleSystemCalls();
    inline static void handleMemAlloc();
    inline static void handleMemFree();
    inline static void handleThreadCreate();
    inline static void handleThreadDispatch();

    static constexpr uint64 SCAUSE_SOFTWARE_INTERRUPT = 0x8000000000000001UL;
    static constexpr uint64 SCAUSE_EXTERNAL_INTERRUPT = 0x8000000000000009UL;
    static constexpr uint64 SCAUSE_ECALL_FROM_USER_MODE = 0x0000000000000008UL;
    static constexpr uint64 SCAUSE_ECALL_FROM_SUPERVISOR_MODE = 0x0000000000000009UL;

    static constexpr uint64 SYS_CALL_MEM_ALLOC = 0x01;
    static constexpr uint64 SYS_CALL_MEM_FREE = 0x02;
    static constexpr uint64 SYS_CALL_THREAD_CREATE = 0x11;
    static constexpr uint64 SYS_CALL_THREAD_DISPATCH = 0x13;
};

inline uint64 Riscv::readScause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::writeScause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::readSepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::writeSepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::readStvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::writeStvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::readStval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::writeStval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::maskSetSip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::maskClearSip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::readSip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::writeSip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::maskSetSstatus(uint64 mask)
{
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::maskClearSstatus(uint64 mask)
{
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::readSstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::writeSstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

#endif //_Riscv_hpp_
