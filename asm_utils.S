.section .text

.globl get_cpu_id

get_cpu_id:
  mrs x0, mpidr_el1
  ret

