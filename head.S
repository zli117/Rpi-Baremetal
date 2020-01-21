#include <memory.h>

.section .text.boot

.globl _begin

_begin:
  # read cpu id. Only initialize bss if on cpu 0
  mrs x1, mpidr_el1
  and x1, x1, #0x3
  cbnz x1, continue 
  # save atag address (device tree)
  mov x5, x0
  # initialize .bss
  ldr x1, =_p_bss_start
  ldr x2, =_p_bss_end
  loop:
    str xzr, [x1]
    add x1, x1, #8
    cmp x1, x2
    b.ne loop
  
  ldr x0, =_begin
  ldr x1, =_start
  str x0, [x1]

  continue:
    # setup cpu ids for each core
    mrs x0, midr_el1
    mrs x1, mpidr_el1
    msr vpidr_el2, x0
    msr vmpidr_el2, x1
    mov x0, #0x33ff
    msr cptr_el2, x0  // Disable coprocessor traps to EL2
    msr hstr_el2, xzr  // Disable coprocessor traps to EL2
    mov x0, #3 << 20
    msr cpacr_el1, x0

    # compute stack starting address for each core
    mrs x1, mpidr_el1
    and x1, x1, #3
    add x1, x1, #1
    mov x2, #STACK_SIZE
    mul x2, x1, x2
    ldr x1, =_p_bin_end
    add x1, x1, x2
    mov sp, x1

    # load parameters for el2_entry
    mrs x1, mpidr_el1
    and x1, x1, #0x3
    mov x0, x5
    bl el2_entry

    # In case el2_entry ever returns
    halt:
      wfe
      b halt
