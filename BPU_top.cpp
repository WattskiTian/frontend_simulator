#include "btb.h"
#include "demo_tage.h"
#include "front_IO.h"
#include "frontend.h"
#include <cstdint>
#include <cstdio>

uint32_t pc_reg;

void BPU_top(struct BPU_in *in, struct BPU_out *out) {
  // generate pc_reg sending to icache
  if (in->reset) {
    pc_reg = RESET_PC;
    return;
  } else if (in->refetch && in->back2front_valid) {
    pc_reg = in->refetch_address;
  } // else pc_reg should be set by the previous cycle

  // send fetch request to icache
  out->icache_read_valid = true; // now always valid
  out->fetch_address = pc_reg;
  out->PTAB_write_enable = true;

  // update branch predictor
  if (in->back2front_valid) {
    TAGE_do_update(in->predict_base_pc, in->actual_dir, out->predict_dir);
    bht_update(in->predict_base_pc, in->actual_dir);
    if (in->actual_dir == true) {
      btb_update(in->predict_base_pc, in->refetch_address, in->actual_br_type,
                 in->actual_dir);
    }
  }

  // do branch prediction
  // traverse instructions in fetch_group, find the first TAGE prediction
  // that is taken
  bool found_taken_branch = false;
  uint32_t branch_pc = pc_reg;

  for (int i = 0; i < FETCH_WIDTH && !found_taken_branch; i++) {
    uint32_t current_pc = pc_reg + (i * 4);
    if (TAGE_get_prediction(current_pc)) {
      found_taken_branch = true;
      branch_pc = current_pc;
    }
  }

  if (found_taken_branch) {
    // only do BTB lookup for taken branches
    uint32_t btb_target = btb_pred(branch_pc);
    out->predict_dir = true;
    out->predict_next_fetch_address = btb_target;
    out->predict_base_pc = branch_pc;
  } else {
    // no prediction for taken branches, execute sequentially
    out->predict_dir = false;
    out->predict_next_fetch_address = pc_reg + (FETCH_WIDTH * 4);
    out->predict_base_pc = pc_reg;
  }
  pc_reg = out->predict_next_fetch_address;
  printf("[BPU_top] icache_fetch_address: %x\n", out->fetch_address);
  printf("[BPU_top] predict_next_fetch_address: %x\n",
         out->predict_next_fetch_address);
  printf("[BPU_top] predict_base_pc: %x\n", out->predict_base_pc);
  printf("[BPU_top] predict_dir: %d\n", out->predict_dir);
}
