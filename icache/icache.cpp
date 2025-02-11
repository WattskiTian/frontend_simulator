#include "../front_IO.h"
#include "../front_module.h"
#include "../frontend.h"
#include <cstdint>
#include <cstdio>
// no actual icache, just a simple simulation

uint32_t pmem[1024];

void icache_top(struct icache_in *in, struct icache_out *out) {
  if (in->reset) {
    DEBUG_LOG("[icache] reset\n");
    return;
  }
  out->icache_read_ready = true;
  // when BPU sends a valid read request
  if (in->icache_read_valid) {
    // read instructions from pmem
    for (int i = 0; i < FETCH_WIDTH; i++) {
      uint32_t pmem_address = in->fetch_address + (i * 4);
      // out->fetch_group[i] = pmem[pmem_address - PMEM_OFFSET];
      DEBUG_LOG("[icache] pmem_address: %x\n", pmem_address);
      out->fetch_group[i] = pmem_address;
    }
  }
}
