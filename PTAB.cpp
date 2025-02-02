#include "front_IO.h"
#include "front_module.h"
#include <queue>

struct PTAB_entry {
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
};

// the FIFO control of PTAB is the same as instruction FIFO !
// the FIFO control of PTAB is the same as instruction FIFO !
// the FIFO control of PTAB is the same as instruction FIFO !
static std::queue<PTAB_entry> ptab;

void PTAB_top(struct PTAB_in *in, struct PTAB_out *out) {
  // dealing with mispredict
  if (in->reset) {
    while (!ptab.empty()) {
      ptab.pop();
    }
    return;
  }

  // when there is new prediction, add it to PTAB
  if (in->write_enable) {
    PTAB_entry entry;
    entry.predict_dir = in->predict_dir;
    entry.predict_next_fetch_address = in->predict_next_fetch_address;
    entry.predict_base_pc = in->predict_base_pc;
    ptab.push(entry);
  }

  // output prediction
  if (in->read_enable) {
    out->predict_dir = ptab.front().predict_dir;
    out->predict_next_fetch_address = ptab.front().predict_next_fetch_address;
    out->predict_base_pc = ptab.front().predict_base_pc;
    ptab.pop();
  }
}