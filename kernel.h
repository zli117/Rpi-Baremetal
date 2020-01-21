#ifndef KERNEL_H
#define KERNEL_H

#include <types.h>

// EL1 entry precondition:
// MMU enabled
// Paging enabled with mapping from 0xffff000000000000 to kernel physical load
// space, and identical mapping to load space is disabled for el1
extern "C" void el1_entry(uint64* v_atag_addr, uint64* v_ttbr1_el1,
                          uint64 cpu_id);

#endif  // KERNEL_H
