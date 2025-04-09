#include "target_cache.h"
#include <cstdint>

static uint32_t bht[BHT_ENTRY_NUM];
static uint32_t target_cache[TC_ENTRY_NUM];

uint32_t bht_cal_idx(uint32_t pc) { return (pc >> 2) % BHT_ENTRY_NUM; }

uint32_t tc_cal_idx(uint32_t bht_val, uint32_t pc) {
  return (bht_val ^ (pc >> 2)) % TC_ENTRY_NUM;
}

uint32_t tc_pred(uint32_t pc) {
  uint32_t bht_idx = bht_cal_idx(pc);
  uint32_t tc_idx = tc_cal_idx(bht[bht_idx], pc);
  return target_cache[tc_idx];
}

void bht_update(uint32_t pc, bool pc_dir) {
  uint32_t bht_idx = bht_cal_idx(pc);
  bht[bht_idx] = (bht[bht_idx] << 1) | pc_dir;
}

void tc_update(uint32_t pc, uint32_t actualAddr) {
  uint32_t bht_idx = bht_cal_idx(pc);
  uint32_t tc_idx = tc_cal_idx(bht[bht_idx], pc);
  target_cache[tc_idx] = actualAddr;
}