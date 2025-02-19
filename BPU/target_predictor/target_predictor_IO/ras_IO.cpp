#include "ras_IO.h"
#include "../../../sequential_components/seq_comp.h"
#include "target_predictor_types.h"

void ras_push(uint32_t addr) {
  if (addr == ras[ras_sp]) {
    ras_cnt[ras_sp]++;
    return;
  }
  ras_sp = (ras_sp + 1) % RAS_ENTRY_NUM;
  ras[ras_sp] = addr;
  ras_cnt[ras_sp] = 1;
}

void ras_push_IO(ras_push_In *in, ras_push_Out *out) {
  if (in->addr == in->ras_read) {
    out->ras_cnt_ctrl = 0;
    out->ras_cnt_wdata = in->ras_cnt_read + 1;
  }
  out->ras_sp_ctrl = 1;
  out->ras_sp_wdata = (in->ras_sp_read + 1) % RAS_ENTRY_NUM;
  out->ras_ctrl = 1;
  out->ras_wdata = in->addr;
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