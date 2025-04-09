#include <cstdint>
#include <cstdio>

#include "../../../sequential_components/seq_comp.h"
#include "target_cache_IO.h"
#include "target_predictor_types.h"

uint32_t bht_cal_idx(uint32_t pc) { return (pc >> 2) & BHT_IDX_MASK; }

uint32_t tc_cal_idx(uint32_t bht_val, uint32_t pc) {
  return (bht_val ^ (pc >> 2)) & TC_IDX_MASK;
}

void tc_pred1(struct tc_pred1_In *in, struct tc_pred1_Out *out) {
  out->bht_idx = bht_cal_idx(in->pc);
}

void tc_pred2(struct tc_pred2_In *in, struct tc_pred2_Out *out) {
  out->tc_idx = tc_cal_idx(in->bht_read, in->pc);
}

uint32_t C_tc_pred_wrapper(uint32_t pc) {
  struct tc_pred1_In *in1 = new tc_pred1_In();
  struct tc_pred1_Out *out1 = new tc_pred1_Out();
  in1->pc = pc;

  tc_pred1(in1, out1);

  struct tc_pred2_In *in2 = new tc_pred2_In();
  struct tc_pred2_Out *out2 = new tc_pred2_Out();
  in2->bht_read = bht[out1->bht_idx];
  in2->pc = pc;

  tc_pred2(in2, out2);

  uint32_t ret = target_cache[out2->tc_idx];
  delete in1;
  delete out1;
  delete in2;
  delete out2;
  return ret;
}

void bht_update_IO(struct bht_update_In *in, struct bht_update_Out *out) {
  out->bht_ctrl = 3;
  out->bht_wdata = ((in->bht_read << 1) | in->pc_dir) & BHT_MASK;
}

void C_bht_update_wrapper(uint32_t pc, bool pc_dir) {
  struct bht_update_In *in = new bht_update_In();
  struct bht_update_Out *out = new bht_update_Out();
  in->pc_dir = pc_dir;
  uint32_t bht_idx = bht_cal_idx(pc);
  in->bht_read = bht[bht_idx];

  bht_update_IO(in, out);

  // update registers
  if (out->bht_ctrl != 0) {
    bht[bht_idx] = out->bht_wdata;
  }
  delete in;
  delete out;
}

void tc_update_IO(struct tc_update_In *in, struct tc_update_Out *out) {
  out->tc_idx = tc_cal_idx(in->bht_read, in->pc);
  out->tc_ctrl = 3;
  out->tc_wdata = in->actualAddr;
}

void C_tc_update_wrapper(uint32_t pc, uint32_t actualAddr) {
  struct tc_update_In *in = new tc_update_In();
  struct tc_update_Out *out = new tc_update_Out();
  uint32_t bht_idx = bht_cal_idx(pc);
  in->bht_read = bht[bht_idx];
  in->pc = pc;
  in->actualAddr = actualAddr;

  tc_update_IO(in, out);

  // update registers
  if (out->tc_ctrl != 0) {
    target_cache[out->tc_idx] = out->tc_wdata;
  }
  delete in;
  delete out;
}
