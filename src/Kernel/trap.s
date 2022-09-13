# 1 "src/Kernel/trap.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/Kernel/trap.S"
.extern handleSupervisorTrap
.global supervisorTrap

.align 4
supervisorTrap:
    addi sp, sp, -248

    # Save all registers on stack except x0, x10 and x11
    # x0 is wired to 0, x10 and x11 are used to get system call return values

    sd x1, 0x00(sp)
    sd x2, 0x08(sp)
    sd x3, 0x10(sp)
    sd x4, 0x18(sp)
    sd x5, 0x20(sp)
    sd x6, 0x28(sp)
    sd x7, 0x30(sp)
    sd x8, 0x38(sp)
    sd x9, 0x40(sp)
    sd x12, 0x48(sp)
    sd x13, 0x50(sp)
    sd x14, 0x58(sp)
    sd x15, 0x60(sp)
    sd x16, 0x68(sp)
    sd x17, 0x70(sp)
    sd x18, 0x78(sp)
    sd x19, 0x80(sp)
    sd x20, 0x88(sp)
    sd x21, 0x90(sp)
    sd x22, 0x98(sp)
    sd x23, 0xA0(sp)
    sd x24, 0xA8(sp)
    sd x25, 0xB0(sp)
    sd x26, 0xB8(sp)
    sd x27, 0xC0(sp)
    sd x28, 0xC8(sp)
    sd x29, 0xD0(sp)
    sd x30, 0xD8(sp)
    sd x31, 0xE0(sp)

    call handleSupervisorTrap

    # Restore all registers

    ld x1, 0x00(sp)
    ld x2, 0x08(sp)
    ld x3, 0x10(sp)
    ld x4, 0x18(sp)
    ld x5, 0x20(sp)
    ld x6, 0x28(sp)
    ld x7, 0x30(sp)
    ld x8, 0x38(sp)
    ld x9, 0x40(sp)
    ld x12, 0x48(sp)
    ld x13, 0x50(sp)
    ld x14, 0x58(sp)
    ld x15, 0x60(sp)
    ld x16, 0x68(sp)
    ld x17, 0x70(sp)
    ld x18, 0x78(sp)
    ld x19, 0x80(sp)
    ld x20, 0x88(sp)
    ld x21, 0x90(sp)
    ld x22, 0x98(sp)
    ld x23, 0xA0(sp)
    ld x24, 0xA8(sp)
    ld x25, 0xB0(sp)
    ld x26, 0xB8(sp)
    ld x27, 0xC0(sp)
    ld x28, 0xC8(sp)
    ld x29, 0xD0(sp)
    ld x30, 0xD8(sp)
    ld x31, 0xE0(sp)

    addi sp, sp, 248

    sret
