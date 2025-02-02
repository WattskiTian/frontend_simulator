#include "btb.h"
#include "demo_tage.h"
#include "front_IO.h"
#include "frontend.h"
#include <cstdint>

uint32_t current_fetch_address; // this should be a register

void BPU_top(struct BPU_in *in, struct BPU_out *out) {
  if (in->reset) {
    current_fetch_address = RESET_PC;
  } else {
    // update branch predictor
    if (in->back2front_valid) {
      TAGE_do_update(in->predict_base_pc, in->actual_dir, out->predict_dir);
      bht_update(in->predict_base_pc, in->actual_dir);
      if (in->actual_dir == true) {
        btb_update(in->predict_base_pc, in->refetch_address, in->actual_br_type,
                   in->actual_dir);
      }
    }
    // do refetch
    if (in->refetch && in->back2front_valid) {
      current_fetch_address = in->refetch_address;
      out->predict_dir = false;
      out->predict_next_fetch_address = in->refetch_address;
      out->predict_base_pc = in->refetch_address;
    }
    // no refetch request, do branch prediction
    else {
      // traverse instructions in fetch_group, find the first TAGE prediction
      // that is taken
      bool found_taken_branch = false;
      uint32_t branch_pc = current_fetch_address;

      for (int i = 0; i < FETCH_WIDTH && !found_taken_branch; i++) {
        uint32_t current_pc = current_fetch_address + (i * 4);
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
        out->predict_next_fetch_address =
            current_fetch_address + (FETCH_WIDTH * 4);
        out->predict_base_pc = current_fetch_address;
      }

      current_fetch_address = out->predict_next_fetch_address;
    }
  }

  // send fetch request to icache
  out->icache_read_valid = true; // now always valid
  out->fetch_address = current_fetch_address;
  out->PTAB_write_enable = true;
}
