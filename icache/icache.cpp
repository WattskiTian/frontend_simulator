#include "./icache.h"

// just a simulation of time delay

// Cache storage
uint32_t icache_tag[ICACHE_WAY_NUM][1 << ICACHE_INDEX_WIDTH];
uint32_t icache_valid[ICACHE_WAY_NUM][1 << ICACHE_INDEX_WIDTH];

// Cache entry structure
struct cache_entry {
  uint32_t instructions[FETCH_WIDTH];
};
struct cache_entry icache_data[ICACHE_WAY_NUM];

// Latency control variables
uint32_t latency_reg = 0;
uint32_t latency_delay_required = 0;
uint8_t icache_state = ICACHE_IDLE; // must be initialized before reset

int icache_select_evict() { return rand() % ICACHE_WAY_NUM; }

void icache_evict(uint32_t addr) {
  int evict_way = icache_select_evict();
  icache_tag[evict_way][ICACHE_INDEX(addr)] = ICACHE_TAG(addr);
  icache_valid[evict_way][ICACHE_INDEX(addr)] = true;
}

int icache_read(uint32_t addr) {
  uint32_t tag;
  int i;

  for (i = 0; i < ICACHE_WAY_NUM; i++) {
    tag = icache_tag[i][ICACHE_INDEX(addr)];
    if (icache_valid[i][ICACHE_INDEX(addr)] && tag == ICACHE_TAG(addr)) {
      break;
    }
  }

  if (i == ICACHE_WAY_NUM) {
    icache_evict(addr);
    DEBUG_LOG("[icache] miss\n");
    return MISS_LATENCY;
  }
  DEBUG_LOG("[icache] hit\n");

  return HIT_LATENCY;
}

void icache_top(struct icache_in *in, struct icache_out *out) {
  if (in->reset) {
    DEBUG_LOG("[icache] reset\n");
    // 重置缓存状态
    for (int i = 0; i < ICACHE_WAY_NUM; i++) {
      for (int j = 0; j < (1 << ICACHE_INDEX_WIDTH); j++) {
        icache_valid[i][j] = false;
      }
    }
    latency_delay_required = 0;
    latency_reg = 0;
    icache_state = ICACHE_IDLE;
    out->icache_read_ready = true;
    out->fetch_group_valid = false;
    return;
  }

  if (icache_state == ICACHE_IDLE) {
    if (in->icache_read_valid) {
      icache_state = ICACHE_WAITING;
      latency_delay_required = icache_read(in->fetch_address);
      out->icache_read_ready = false;
      out->fetch_group_valid = false;
      latency_reg++;
      // dealing with no latency case
      if (latency_delay_required == 0) {
        latency_delay_required = 0;
        latency_reg = 0;
        icache_state = ICACHE_IDLE;
        out->icache_read_ready = true;
        out->fetch_group_valid = true;
        for (int i = 0; i < FETCH_WIDTH; i++) {
          uint32_t pmem_address = in->fetch_address + (i * 4);
          DEBUG_LOG("[icache] pmem_address: %x\n", pmem_address);
          out->fetch_group[i] = pmem_address;
        }
      }
      return;
    }
  } else if (icache_state == ICACHE_WAITING) {
    if (latency_reg >= latency_delay_required) {
      latency_delay_required = 0;
      latency_reg = 0;
      icache_state = ICACHE_IDLE;
      out->icache_read_ready = true;
      out->fetch_group_valid = true;
      for (int i = 0; i < FETCH_WIDTH; i++) {
        uint32_t pmem_address = in->fetch_address + (i * 4);
        DEBUG_LOG("[icache] pmem_address: %x\n", pmem_address);
        out->fetch_group[i] = pmem_address;
      }
      return;
    }
    // still waiting
    out->icache_read_ready = false;
    out->fetch_group_valid = false;
    latency_reg++;
    return;
  }
}
