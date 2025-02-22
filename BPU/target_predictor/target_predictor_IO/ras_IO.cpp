#include "ras_IO.h"
#include "../../../sequential_components/seq_comp.h"
#include "target_predictor_types.h"
#include <cstdint>

void ras_push(uint32_t addr) {
  if (addr == ras[ras_sp]) {
    ras_cnt[ras_sp]++;
    return;
  }
  ras_sp = (ras_sp + 1) % RAS_ENTRY_NUM;
  ras[ras_sp] = addr;
  ras_cnt[ras_sp] = 1;
}

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
  out->ras_sp_wdata = (in->ras_sp_read + 1) % RAS_ENTRY_NUM;
  out->ras_ctrl = 3;
  out->ras_wdata = in->addr;
  out->ras_cnt_ctrl = 3;
  out->ras_cnt_wdata = 1;
}

struct ras_push_In ras_push_in;
struct ras_push_Out ras_push_out;

void C_ras_push(uint32_t addr) {
  struct ras_push_In *in = &ras_push_in;
  struct ras_push_Out *out = &ras_push_out;
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
  return;
}

uint32_t ras_pop() {
  if (ras_cnt[ras_sp] > 1) {
    ras_cnt[ras_sp]--;
    return ras[ras_sp];
  } else if (ras_cnt[ras_sp] == 1) {
    ras_cnt[ras_sp] = 0;
    ras_sp = (ras_sp + RAS_ENTRY_NUM - 1) % RAS_ENTRY_NUM;
    return ras[ras_sp + 1];
  } else
    return -1; // null on top
}