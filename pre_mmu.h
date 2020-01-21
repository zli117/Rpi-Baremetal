#ifndef PRE_MMU_H
#define PRE_MMU_H

#include <types.h>

// EL2 entry precondition (same as Linux precondition):
// MMU = off
// D-cache = off
// I-cache = on or off
// Interrupt = distabled
extern "C" void el2_entry(uint64* p_atag_addr, uint64 cpu_id);

#endif  // PRE_MMU_H
