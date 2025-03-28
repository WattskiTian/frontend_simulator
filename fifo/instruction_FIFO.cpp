#include "../front_IO.h"
#include "../frontend.h"
#include <array>
#include <assert.h>
#include <cstdio>
#include <queue>

#define FIFO_SIZE 80000

struct FIFO_entry {
  std::array<uint32_t, FETCH_WIDTH> instructions;
};

std::queue<FIFO_entry> fifo;

void instruction_FIFO_top(struct instruction_FIFO_in *in,
                          struct instruction_FIFO_out *out) {
  // clear FIFO
  if (in->reset) {
    DEBUG_LOG("[FIFO] reset\n");
    while (!fifo.empty()) {
      fifo.pop();
    }
    out->full = false;
    out->empty = true;
    out->FIFO_valid = false;
    return;
  }
  if (in->refetch) {
    while (!fifo.empty()) {
      fifo.pop();
    }
    out->full = false;
    out->empty = true;
    out->FIFO_valid = false;
  }

  if (fifo.size() >= FIFO_SIZE && in->write_enable) {
    assert(0);
  }

  // if FIFO is not full and icache has new data
  if (fifo.size() < FIFO_SIZE && in->write_enable) {
    FIFO_entry entry;
    for (int i = 0; i < FETCH_WIDTH; i++) {
      entry.instructions[i] = in->fetch_group[i];
    }
    fifo.push(entry);
    DEBUG_LOG("[FIFO] fifo pushing, size: %lu\n", fifo.size());
  }

  // output data
  if (!fifo.empty() && in->read_enable) {
    for (int i = 0; i < FETCH_WIDTH; i++) {
      out->instructions[i] = fifo.front().instructions[i];
    }
    fifo.pop();
    DEBUG_LOG("[FIFO] fifo popping, size: %lu\n", fifo.size());
    out->FIFO_valid = true;
  } else {
    out->FIFO_valid = false;
  }

  out->empty = fifo.empty();
  out->full = fifo.size() == FIFO_SIZE;
  // DEBUG_LOG("[FIFO] fifo size: %lu\n", fifo.size());
}
