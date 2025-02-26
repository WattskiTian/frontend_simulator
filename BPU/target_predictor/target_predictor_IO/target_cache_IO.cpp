#include "target_cache_IO.h"
#include "../../../frontend.h"
#include "../../../sequential_components/seq_comp.h"
#include "target_predictor_types.h"
#include <cstdint>
#include <cstdio>
uint32_t tc_pred(uint32_t pc) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  uint32_t tc_idx = (bht[bht_idx] ^ pc) % TC_ENTRY_NUM;
  return target_cache[tc_idx];
}

void tc_pred1(struct tc_pred1_In *in, struct tc_pred1_Out *out) {
  out->bht_idx = in->pc % BHT_ENTRY_NUM;
}

void tc_pred2(struct tc_pred2_In *in, struct tc_pred2_Out *out) {
  out->tc_idx = (in->bht_read ^ in->pc) % TC_ENTRY_NUM;
}

struct tc_pred1_In tc_pred1_in;
struct tc_pred1_Out tc_pred1_out;
struct tc_pred2_In tc_pred2_in;
struct tc_pred2_Out tc_pred2_out;

uint32_t C_tc_pred_wrapper(uint32_t pc) {
  struct tc_pred1_In *in1 = &tc_pred1_in;
  struct tc_pred1_Out *out1 = &tc_pred1_out;
  in1->pc = pc;
  tc_pred1(in1, out1);
  struct tc_pred2_In *in2 = &tc_pred2_in;
  struct tc_pred2_Out *out2 = &tc_pred2_out;
  in2->bht_read = bht[out1->bht_idx];
  in2->pc = pc;
  tc_pred2(in2, out2);
  // #ifdef IO_GEN_MODE
  //   if (io_gen_cnt >= 0) {
  //     printf("tc");
  //     printf("%d ", bht[out1->bht_idx]);
  //     printf("%d ", target_cache[out2->tc_idx]);
  //     printf("\n");
  //   }
  // #endif
  return target_cache[out2->tc_idx];
}

void bht_update(uint32_t pc, bool pc_dir) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  bht[bht_idx] = (bht[bht_idx] << 1) | pc_dir;
}

void bht_update_IO(struct bht_update_In *in, struct bht_update_Out *out) {
  out->bht_ctrl = 3;
  out->bht_wdata = (in->bht_read << 1) | in->pc_dir;
}

struct bht_update_In bht_update_in;
struct bht_update_Out bht_update_out;
void C_bht_update_wrapper(uint32_t pc, bool pc_dir) {
  struct bht_update_In *in = &bht_update_in;
  struct bht_update_Out *out = &bht_update_out;
  in->pc_dir = pc_dir;
  in->bht_read = bht[pc % BHT_ENTRY_NUM];
  bht_update_IO(in, out);
  // update registers
  if (out->bht_ctrl != 0) {
    bht[pc % BHT_ENTRY_NUM] = out->bht_wdata;
  }
}

void tc_update(uint32_t pc, uint32_t actualAddr) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  uint32_t tc_idx = (bht[bht_idx] ^ pc) % TC_ENTRY_NUM;
  target_cache[tc_idx] = actualAddr;
}

void tc_update_IO(struct tc_update_In *in, struct tc_update_Out *out) {
  out->tc_idx = (in->bht_read ^ in->pc) % TC_ENTRY_NUM;
  out->tc_ctrl = 3;
  out->tc_wdata = in->actualAddr;
}

struct tc_update_In tc_update_in;
struct tc_update_Out tc_update_out;
void C_tc_update_wrapper(uint32_t pc, uint32_t actualAddr) {
  struct tc_update_In *in = &tc_update_in;
  struct tc_update_Out *out = &tc_update_out;
  in->bht_read = bht[pc % BHT_ENTRY_NUM];
  in->pc = pc;
  in->actualAddr = actualAddr;
  tc_update_IO(in, out);
  // update registers
  if (out->tc_ctrl != 0) {
    target_cache[out->tc_idx] = out->tc_wdata;
  }
}
