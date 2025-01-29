#ifndef FRONT_IO_H
#define FRONT_IO_H

#include "frontend.h"
#include <cstdint>

struct front_top_in {
  bool reset;
  // from back-end
  bool back2front_valid;
  bool refetch_valid;
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
  bool refetch_valid;
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
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
};

struct icache_in {
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
  // from icache
  bool icache_read_ready;
  uint32_t fetch_group[FETCH_WIDTH];
  // from back-end
  bool mispredict;
  bool FIFO_read_enable;
};

struct instruction_FIFO_out {
  // to icache
  bool FIFO_ready;
  // to back-end
  bool FIFO_valid;
  uint32_t instructions[FETCH_WIDTH];
  // to PTAB
  bool PTAB_read_enable;
  bool PTAB_write_enable;
};

struct PTAB_in {
  // from instruction FIFO
  bool PTAB_read_enable;
  bool PTAB_write_enable;
  // from BPU
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
  // from back-end
  bool mispredict;
};

struct PTAB_out {
  // to back-end
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
};
#endif
