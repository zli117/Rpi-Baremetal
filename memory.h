#ifndef MEMORY_H
#define MEMORY_H

#define STACK_SIZE 4096

#define TWO_MB 0x200000
#define READ_WRITE (1 << 6)
#define OUTER_SHARABLE (0b10 << 8)
#define AF (1 << 10)
#define NG (1 << 11)
#define CONTIGUOUS ((uint64)1 << 52)
#define PXN ((uint64)1 << 53)
#define XN ((uint64)1 << 54)
#define BLOCK 0x1

#define NON_CACHABLE 0b01000100
#define DEVICE 0b00000000

#define TCR_T0SZ ((64 - 48))
#define TCR_T1SZ ((64 - 48) << 16)
#define TCR_TG0 ((0b00 << 14))
#define TCR_TG1 ((0b00 << 30))

#define P_2_V(p_addr) ((uint64)(p_addr) + 0xffff000000000000)
#define V_2_P(v_addr) ((uint64)(v_addr) & (~0xffff000000000000))

#endif  // MEMORY_H