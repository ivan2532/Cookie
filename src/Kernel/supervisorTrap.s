# 1 "src/Kernel/supervisorTrap.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/Kernel/supervisorTrap.S"
.extern _ZN5Riscv15handleEcallTrapEv
.extern _ZN5Riscv15handleTimerTrapEv
.extern _ZN5Riscv18handleExternalTrapEv

.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:
    j ecallTrap
    j timerTrap
    .skip 28
    j externalTrap

ecallTrap:
    addi sp, sp, -256
    .irp index 1,2,3,4,5,6,7,8,9,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5Riscv15handleEcallTrapEv

    .irp index 1,2,3,4,5,6,7,8,9,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    sret

timerTrap:
    addi sp, sp, -256
    .irp index 1,2,3,4,5,6,7,8,9,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5Riscv15handleTimerTrapEv

    .irp index 1,2,3,4,5,6,7,8,9,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    sret

externalTrap:
    addi sp, sp, -256
    .irp index 1,2,3,4,5,6,7,8,9,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5Riscv18handleExternalTrapEv

    .irp index 1,2,3,4,5,6,7,8,9,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    sret
