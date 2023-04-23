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

    // Push x3..x31 registers onto stack
    static void pushRegisters();
    //_ZN5Riscv13pushRegistersEv

    // Pop x3..x31 registers onto stack
    static void popRegisters();

    // Read scause
    static uint64 r_scause();

    // Write to scause
    static void w_scause(uint64 scause);

    // Read sepc
    static uint64 r_sepc();

    // Write to sepc
    static void w_sepc(uint64 sepc);

    // Read stvec
    static uint64 r_stvec();

    // Write to stvec
    static void w_stvec(uint64 stvec);

    // Read stval
    static uint64 r_stval();

    // Write to stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    // Mask set sip
    static void ms_sip(uint64 mask);

    // Clears only the bits covered by the mask in sip register
    static void mc_sip(uint64 mask);

    // Read sip
    static uint64 r_sip();

    // Write sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // Mask set sstatus
    static void ms_sstatus(uint64 mask);

    // Mask clear sstatus
    static void mc_sstatus(uint64 mask);

    // Read sstatus
    static uint64 r_sstatus();

    // Write to sstatus
    static void w_sstatus(uint64 sstatus);

    static void supervisorTrap();

private:
    static void handleSupervisorTrap();
    inline static void handleSoftwareInterrupt();
    inline static void handleExternalInterrupt();
    inline static void handleEcall();
    inline static void handleUnknownTrapCause(uint64 scause);
    inline static void handleSystemCalls();

    // Timer interrupt is passed on as a software interrupt
    static constexpr uint64 SCAUSE_SOFTWARE_INTERRUPT = 0x8000000000000001UL;
    static constexpr uint64 SCAUSE_EXTERNAL_INTERRUPT = 0x8000000000000009UL;
    static constexpr uint64 SCAUSE_ECALL_FROM_USER_MODE = 0x0000000000000008UL;
    static constexpr uint64 SCAUSE_ECALL_FROM_SUPERVISOR_MODE = 0x0000000000000009UL;
};

inline uint64 Riscv::r_scause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_stval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::ms_sip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::ms_sstatus(uint64 mask)
{
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask)
{
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

#endif //_Riscv_hpp_
