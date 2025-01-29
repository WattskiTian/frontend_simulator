#include "front_IO.h"
#include "front_module.h"
#include "frontend.h"
#include <cstdint>
// no actual icache, just a simple simulation

#define PMEM_OFFSET 0x80000000

uint32_t pmem[1024];

void icache_top(struct icache_in *in, struct icache_out *out) {
  // when BPU sends a valid read request
  if (in->icache_read_valid) {
    // read instructions from pmem
    for (int i = 0; i < FETCH_WIDTH; i++) {
      uint32_t pmem_address = in->fetch_address + (i * 4);
      out->fetch_group[i] = pmem[pmem_address - PMEM_OFFSET];
    }
    out->icache_read_ready = true;
  } else {
    out->icache_read_ready = false;
  }
}
