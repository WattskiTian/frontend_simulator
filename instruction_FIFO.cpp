#include "front_IO.h"
#include "front_module.h"
#include <array>
#include <queue>

#define FIFO_SIZE 8

struct FIFO_entry {
  std::array<uint32_t, FETCH_WIDTH> instructions;
};

static std::queue<FIFO_entry> fifo;

void instruction_FIFO_top(struct instruction_FIFO_in *in,
                          struct instruction_FIFO_out *out) {
  // dealing with mispredict
  // clear FIFO
  if (in->mispredict) {
    while (!fifo.empty()) {
      fifo.pop();
    }
    out->FIFO_valid = false;
    out->FIFO_ready = true;
    return;
  }

  // if FIFO is not full and icache has new data
  if (fifo.size() < FIFO_SIZE && in->icache_read_ready) {
    FIFO_entry entry;
    for (int i = 0; i < FETCH_WIDTH; i++) {
      entry.instructions[i] = in->fetch_group[i];
    }
    fifo.push(entry);
    out->PTAB_write_enable = true;
  } else {
    out->PTAB_write_enable = false;
  }

  // set ready signal
  out->FIFO_ready = (fifo.size() < FIFO_SIZE);

  // output data
  if (!fifo.empty() && in->FIFO_read_enable) {
    out->FIFO_valid = true;
    for (int i = 0; i < FETCH_WIDTH; i++) {
      out->instructions[i] = fifo.front().instructions[i];
    }
    fifo.pop();
    out->PTAB_read_enable = true;
  } else {
    out->FIFO_valid = false;
    out->PTAB_read_enable = false;
  }
}