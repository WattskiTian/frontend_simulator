#include "target_cache_IO.h"
#include "../../../sequential_components/seq_comp.h"
#include "target_predictor_types.h"
#include <cstdint>

uint32_t tc_pred(uint32_t pc) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  uint32_t tc_idx = (bht[bht_idx] ^ pc) % TC_ENTRY_NUM;
  return target_cache[tc_idx];
}

void C_tc_pred(struct tc_pred_In *in, struct tc_pred_Out *out) {}

void bht_update(uint32_t pc, bool pc_dir) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  bht[bht_idx] = (bht[bht_idx] << 1) | pc_dir;
}

void tc_update(uint32_t pc, uint32_t actualAddr) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  uint32_t tc_idx = (bht[bht_idx] ^ pc) % TC_ENTRY_NUM;
  target_cache[tc_idx] = actualAddr;
}