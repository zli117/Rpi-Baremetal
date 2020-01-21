#include <kernel.h>
#include <pre_mmu.h>
#include <memory.h>

extern uint64 _p_bin_end;
extern uint64 _virtual_offset;

// Initial paging mapping uses 4kb granularity, mapping to 2MB blocks
// Also low address only spans 1G
static uint64 level2_table[512] __attribute__((aligned(4096))) = {0};

void el2_entry(uint64* p_atag_addr, uint64 cpu_id) {
  uint64 ID_AA64MMFR0_EL1_val;
  asm("mrs %0, ID_AA64MMFR0_EL1" : "=r"(ID_AA64MMFR0_EL1_val));
  if (((ID_AA64MMFR0_EL1_val >> 28) & 0x0F) != 0) {
    // 4kb granule is not supported
    return;
  }
  uint64 end_page = _p_bin_end & (TWO_MB - 1);
  for (uint64 page_index = 0; page_index <= end_page; ++page_index) {
    level2_table[page_index] = (page_index << 21) | CONTIGUOUS | NG | AF |
                               OUTER_SHARABLE | READ_WRITE | BLOCK;
  }

  // Set attr0 in MAIR_EL1 to be non cachable
  uint64 mair_el1 = NON_CACHABLE | (DEVICE << 8);
  // 48 bits virtual address and 4KB for TTBR0 and TTBR1
  uint64 tcr_el1 = TCR_T0SZ | TCR_T1SZ | TCR_TG0 | TCR_TG1;

  asm("msr MAIR_EL1, %0\n"
      "msr TCR_EL1, %1\n"
      "msr TTBR0_EL1, %2\n"
      "msr TTBR1_EL1, %2\n"
      "isb\n"
      "mrs x0, SCTLR_EL1\n"
      "orr x0, x0, #1\n"
      "msr SCTLR_EL1, x0\n"
      "isb\n"
      "tlbi alle1\n"  // Invalidate TLB
      :
      : "r"(mair_el1), "r"(tcr_el1), "r"(level2_table)
      : "x0");

  // At this point MMU should be enabled for EL1

  // Return to EL1
  asm volatile(
      "ldr x0, =to_el1\n"
      "msr ELR_EL2, x0\n"
      "eret\n"
      "to_el1:\n"
      :
      :
      : "x0");

  // el1_entry should never return
  uint64 v_atag_addr = (uint64)p_atag_addr + 0xffff000000000000;
  uint64 v_ttbr1_el1 = (uint64)level2_table + 0xffff000000000000;
  el1_entry((uint64*)v_atag_addr, (uint64*)v_ttbr1_el1, cpu_id);
}
