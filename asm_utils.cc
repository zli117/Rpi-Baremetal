#include <asm_utils.h>

uint64 get_cpu_id() {
  uint64 cpu_id;
  asm("mrs %0, mpidr_el1\n"
      "and %0, %0, #0x3\n"
      : "=r"(cpu_id));
  return cpu_id;
}
