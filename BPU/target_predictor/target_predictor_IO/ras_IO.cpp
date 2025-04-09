#include <cstdint>
#include <cstdio>

#include "../../../sequential_components/seq_comp.h"
#include "ras_IO.h"
#include "target_predictor_types.h"

// ctrl : remain/inc/dec/alloc : 0123
void ras_push_IO(struct ras_push_In *in, struct ras_push_Out *out) {
  // reset all outputs to prevent previous values
  out->ras_cnt_ctrl = 0;
  out->ras_cnt_wdata = 0;
  out->ras_sp_ctrl = 0;
  out->ras_sp_wdata = 0;
  out->ras_ctrl = 0;
  out->ras_wdata = 0;

  if (in->addr == in->ras_read) {
    out->ras_cnt_ctrl = 1;
    out->ras_cnt_wdata = in->ras_cnt_read + 1;
    return;
  }
  out->ras_sp_ctrl = 3;
  out->ras_sp_wdata = ((in->ras_sp_read + 1) % RAS_ENTRY_NUM);
  out->ras_ctrl = 3;
  out->ras_wdata = in->addr;
  out->ras_cnt_ctrl = 3;
  out->ras_cnt_wdata = 1;
}

void C_ras_push_wrapper(uint32_t addr) {
  // printf("ras_sp=%d\n", ras_sp);
  ras_sp %= RAS_ENTRY_NUM;
  struct ras_push_In *in = new ras_push_In();
  struct ras_push_Out *out = new ras_push_Out();
  in->addr = addr;
  in->ras_read = ras[ras_sp];
  in->ras_cnt_read = ras_cnt[ras_sp];
  in->ras_sp_read = ras_sp;

  ras_push_IO(in, out);

  if (out->ras_cnt_ctrl != 0) {
    ras_cnt[out->ras_sp_wdata] = out->ras_cnt_wdata;
  }
  if (out->ras_sp_ctrl != 0) {
    ras_sp = out->ras_sp_wdata;
  }
  if (out->ras_ctrl != 0) {
    ras[out->ras_sp_wdata] = out->ras_wdata;
  }
  delete in;
  delete out;
  return;
}

// ctrl : remain/inc/dec/alloc : 0123
void ras_pop_IO(struct ras_pop_In *in, struct ras_pop_Out *out) {
  // reset all outputs to prevent previous values
  out->ras_cnt_ctrl = 0;
  out->ras_cnt_wdata = 0;
  out->ras_sp_ctrl = 0;
  out->ras_sp_wdata = 0;
  out->ras_pop_value = 0;

  if (in->ras_cnt_read > 1) {
    out->ras_cnt_ctrl = 2;
    out->ras_cnt_wdata = in->ras_cnt_read - 1;
    out->ras_pop_value = in->ras_read;
    return;
  }
  if (in->ras_cnt_read == 1) {
    out->ras_cnt_ctrl = 2;
    out->ras_cnt_wdata = 0;
    out->ras_sp_ctrl = 3;
    out->ras_sp_wdata = ((in->ras_sp_read + RAS_ENTRY_NUM - 1) % RAS_ENTRY_NUM);
    out->ras_pop_value = in->ras_read;
    return;
  }
  out->ras_pop_value = -1;
  return;
}

uint32_t C_ras_pop_wrapper() {
  // printf("ras_sp=%d\n", ras_sp);
  ras_sp %= RAS_ENTRY_NUM;
  struct ras_pop_In *in = new ras_pop_In();
  struct ras_pop_Out *out = new ras_pop_Out();
  in->ras_read = ras[ras_sp];
  in->ras_cnt_read = ras_cnt[ras_sp];
  in->ras_sp_read = ras_sp;

  ras_pop_IO(in, out);

  if (out->ras_cnt_ctrl != 0) {
    ras_cnt[in->ras_sp_read] = out->ras_cnt_wdata;
  }
  if (out->ras_sp_ctrl != 0) {
    ras_sp = out->ras_sp_wdata;
  }
  uint32_t ret = out->ras_pop_value;
  delete in;
  delete out;
  return ret;
}