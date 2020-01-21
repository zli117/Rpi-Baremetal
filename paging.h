#ifndef PAGING_H
#define PAGING_H

#include <types.h>

void map_static_table_64k(uint64 p_address, uint64 v_address,
                          uint64 continuous_pages);

#endif  // PAGING_H
