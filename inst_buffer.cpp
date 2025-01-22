#include <cstdint>

#include "frontend.h"

#define INST_BUFFER_ENTRY_NUM 8
bool inst_buffer_valid[INST_BUFFER_ENTRY_NUM];
uint32_t inst_buffer_pc[INST_BUFFER_ENTRY_NUM][FETCH_WIDTH];
uint32_t inst_buffer_pred_dir[INST_BUFFER_ENTRY_NUM][FETCH_WIDTH];
uint32_t inst_buffer_pred_target_addr[INST_BUFFER_ENTRY_NUM][FETCH_WIDTH];
uint32_t inst_buffer_instruction[INST_BUFFER_ENTRY_NUM][FETCH_WIDTH];

bool check_inst_buffer_empty() {
  for (int i = 0; i < INST_BUFFER_ENTRY_NUM; i++) {
    if (inst_buffer_valid[i] == false) {
      return true;
    }
  }
  return false;
}

int inst_buffer_write(uint32_t pc, bool pred_dir, uint32_t pred_target_addr,
                      uint32_t instruction) {
  for (int i = 0; i < INST_BUFFER_ENTRY_NUM; i++) {
    if (!inst_buffer_valid[i]) {
      for (int j = 0; j < FETCH_WIDTH; j++) {
        inst_buffer_pc[i][j] = pc;
        inst_buffer_pred_dir[i][j] = pred_dir;
        inst_buffer_pred_target_addr[i][j] = pred_target_addr;
        inst_buffer_instruction[i][j] = instruction;
      }
      return i;
    }
  }
  return -1;
}
