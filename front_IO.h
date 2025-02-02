#ifndef FRONT_IO_H
#define FRONT_IO_H

#include "frontend.h"
#include <cstdint>

struct front_top_in {
  bool reset;
  // from back-end
  bool back2front_valid;
  bool refetch;
  uint32_t predict_base_pc;
  uint32_t refetch_address;
  bool predict_dir;
  bool actual_dir;
  uint32_t actual_br_type;
  bool FIFO_read_enable;
};

struct front_top_out {
  // to back-end
  bool FIFO_valid;
  uint32_t instructions[FETCH_WIDTH];
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
};

struct BPU_in {
  bool reset;
  // from back-end
  bool back2front_valid;
  bool refetch;
  uint32_t predict_base_pc;
  uint32_t refetch_address;
  bool predict_dir;
  bool actual_dir;
  uint32_t actual_br_type;
  // from icache
  bool icache_read_ready;
};

struct BPU_out {
  // to icache
  bool icache_read_valid;
  uint32_t fetch_address;
  // to PTAB
  bool PTAB_write_enable;
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
};

struct icache_in {
  bool reset;
  // from BPU
  bool icache_read_valid;
  uint32_t fetch_address;
};

struct icache_out {
  // to BPU & instruction FIFO
  bool icache_read_ready;
  // to instruction FIFO
  uint32_t fetch_group[FETCH_WIDTH];
};

struct instruction_FIFO_in {
  bool reset; // reset or mispredict
  // from icache
  bool write_enable;
  uint32_t fetch_group[FETCH_WIDTH];
  // from back-end
  bool read_enable;
};

struct instruction_FIFO_out {
  bool full;
  bool empty;
  // to back-end
  bool FIFO_valid;
  uint32_t instructions[FETCH_WIDTH];
};

struct PTAB_in {
  bool reset; // reset or mispredict
  // from BPU
  bool write_enable;
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
  // from back-end
  bool read_enable;
};

struct PTAB_out {
  bool full;
  bool empty;
  // to back-end
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
};
#endif
