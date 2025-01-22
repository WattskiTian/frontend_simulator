#include "frontend.h"
#include <cstdint>
// no actual icache, just a simple simulation

#define PMEM_OFFSET 0x80000000

uint32_t icache_instruction[FETCH_WIDTH];
uint32_t pmem[1024]; // just a placeholder

void icache_read(uint32_t fetch_address) {
  for (int i = 0; i < FETCH_WIDTH; i++) {
    uint32_t pmem_address = fetch_address + (i * 4);
    icache_instruction[i] = pmem[pmem_address - PMEM_OFFSET];
  }
}
