#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sys/types.h>

#define BTB_ENTRY_NUM 64
#define BTB_TAG_LEN 8

uint32_t btb_tag[BTB_ENTRY_NUM];
uint32_t btb_bta[BTB_ENTRY_NUM];
bool btb_valid[BTB_ENTRY_NUM];

uint32_t btb_get_tag(uint32_t pc) { return pc & ((1 << BTB_TAG_LEN) - 1); }
uint32_t btb_get_idx(uint32_t pc) {
  return (pc >> BTB_TAG_LEN) % BTB_ENTRY_NUM;
}

uint32_t btb_pred(uint32_t pc) {
  uint32_t idx = btb_get_idx(pc);
  uint32_t tag = btb_get_tag(pc);
  if (tag == btb_tag[idx] && btb_valid[idx] == true) {
    return btb_bta[idx];
  }
  return pc + 4;
}

void btb_update(uint32_t pc, uint32_t actualAddr) {
  uint32_t idx = btb_get_idx(pc);
  uint32_t tag = btb_get_tag(pc);
  btb_valid[idx] = true;
  btb_bta[idx] = actualAddr;
  btb_tag[idx] = tag;
}

using namespace std;

ifstream log_file;

uint32_t log_pc;
uint32_t log_nextpc;
bool log_bp;
bool show_details = false;

uint32_t log_pc_buffer = -1;

void log_read() {
  char pc_str[40];
  if (log_pc_buffer == -1) {
    log_file.getline(pc_str, 40);
    log_pc_buffer = 0;
    for (int i = 0; i < 8; i++) {
      if (i != 0)
        log_pc_buffer = log_pc_buffer << 4;
      log_pc_buffer += pc_str[i] - (pc_str[i] >= 'a' ? ('a' - 10) : '0');
    }
  }

  char branch_taken_str[5];
  log_file.getline(branch_taken_str, 5);
  log_file.getline(pc_str, 40);

  log_pc = log_pc_buffer;

  log_nextpc = 0;
  log_bp = true;
  for (int i = 0; i < 8; i++) {
    if (i != 0)
      log_nextpc = log_nextpc << 4;
    log_nextpc += pc_str[i] - (pc_str[i] >= 'a' ? ('a' - 10) : '0');
  }
  log_pc_buffer = log_nextpc;
  log_bp = branch_taken_str[0] - '0';
  if (show_details == true)
    printf("pc = %08x bp = %b log_nextpc=%08x\n", log_pc, log_bp, log_nextpc);
}

#define DEBUG false
uint64_t bp_cnt = 0;
uint64_t btb_hit = 0;
int main() {
  log_file.open("../../rv-simu/log");
  int log_pc_max = DEBUG ? 10 : 1000000;
  while (log_pc_max--) {
    log_read();

    uint32_t btb_res = 0;
    if (log_bp == true) {
      bp_cnt++;
      btb_res = btb_pred(log_pc);
      if (btb_res == log_nextpc)
        btb_hit++;
      btb_update(log_pc, log_nextpc);
    }
  }
  double acc = (double)btb_hit / bp_cnt;
  printf("[version btb]     branch_cnt= %lu btb_hit = %lu ACC = %.3f%%\n",
         bp_cnt, btb_hit, acc * 100);
  return 0;
}
