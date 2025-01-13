#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sys/types.h>

#define BTB_ENTRY_NUM 2048
#define BTB_TAG_LEN 8

uint32_t btb_tag[BTB_ENTRY_NUM];
uint32_t btb_bta[BTB_ENTRY_NUM];
bool btb_valid[BTB_ENTRY_NUM];
uint32_t btb_br_type[BTB_ENTRY_NUM];

#define BR_DIRECT 0
#define BR_CALL 1
#define BR_RET 2
#define BR_IDIRECT 3

// RAS
// TODO:RAS_MAX_DEAL
#define RAS_ENTRY_NUM 2048
#define RAS_CNT_LEN 8 // cnt for repeated call
uint32_t ras[RAS_ENTRY_NUM];
uint32_t ras_cnt[RAS_ENTRY_NUM];
uint32_t ras_sp;

// TARGET_CAHCE
#define TC_ENTRY_NUM 2048
#define BHT_ENTRY_NUM 2048
#define BHT_LEN 32
uint32_t bht[BHT_ENTRY_NUM];
uint32_t target_cache[TC_ENTRY_NUM];

uint32_t tc_pred(uint32_t pc) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  uint32_t tc_idx = (bht[bht_idx] ^ pc) % TC_ENTRY_NUM;
  return target_cache[tc_idx];
}

void tc_update(uint32_t pc, bool pc_dir, uint32_t actualAddr) {
  uint32_t bht_idx = pc % BHT_ENTRY_NUM;
  uint32_t tc_idx = (bht[bht_idx] ^ pc) % TC_ENTRY_NUM;
  bht[bht_idx] = (bht[bht_idx] << 1) | pc_dir;
  target_cache[tc_idx] = actualAddr;
}

void ras_push(uint32_t addr) {
  if (addr == ras[ras_sp]) {
    ras_cnt[ras_sp]++;
    return;
  }
  ras_sp++;
  ras[ras_sp] = addr;
  ras_cnt[ras_sp] = 1;
}

uint32_t ras_pop() {
  if (ras_cnt[ras_sp] > 1) {
    ras_cnt[ras_sp]--;
    return ras[ras_sp];
  } else if (ras_cnt[ras_sp] == 1) {
    ras_cnt[ras_sp]--;
    ras_sp--;
    return ras[ras_sp + 1];
  } else
    return -1; // null on top
}

uint32_t btb_get_tag(uint32_t pc) { return pc & ((1 << BTB_TAG_LEN) - 1); }
uint32_t btb_get_idx(uint32_t pc) {
  return (pc >> BTB_TAG_LEN) % BTB_ENTRY_NUM;
}

uint32_t btb_pred(uint32_t pc) {
  uint32_t idx = btb_get_idx(pc);
  uint32_t tag = btb_get_tag(pc);
  uint32_t br_type = btb_br_type[idx];
  if (tag != btb_tag[idx] || btb_valid[idx] != true)
    return -1;

  if (br_type == BR_DIRECT) {
    return btb_bta[idx];
  } else if (br_type == BR_CALL) {
    ras_push(pc + 4);
    return btb_bta[idx];
  } else if (br_type == BR_RET) {
    return ras_pop();
  } else
    return tc_pred(pc);
}

void btb_update(uint32_t pc, uint32_t actualAddr, uint32_t br_type,
                bool actualdir) {
  uint32_t idx = btb_get_idx(pc);
  uint32_t tag = btb_get_tag(pc);
  btb_valid[idx] = true;
  btb_bta[idx] = actualAddr;
  btb_tag[idx] = tag;
  btb_br_type[idx] = br_type;
  if (br_type == BR_IDIRECT) {
    tc_update(pc, actualdir, actualAddr);
  }
}

using namespace std;

// file data
FILE *log_file;
bool log_dir;
uint32_t log_pc;
uint32_t log_nextpc;
uint32_t log_br_type;
bool show_details = false;

uint32_t log_pc_buffer = -1;

void readFileData() {
  uint32_t num1, num2, num3, num4;
  if (fscanf(log_file, "%u %x %x %u\n", &num1, &num2, &num3, &num4) == 4) {
    /*printf("%u 0x%08x 0x%08x %u\n", num1, num2, num3, num4);*/
    log_dir = (bool)num1;
    log_pc = num2;
    log_nextpc = num3;
    log_br_type = num4;
    /*printf("%u 0x%08x 0x%08x %u\n", log_dir, log_pc, log_nextpc,
     * log_br_type);*/
  } else {
    printf("ops");
  }
}

#define DEBUG true
uint64_t bp_cnt = 0;
uint64_t btb_hit = 0;
int main() {
  log_file = fopen("/home/watts/dhrystone/gem5output_rv/fronted_log", "r");
  if (log_file == NULL) {
    printf("log_file open error\n");
    return 0;
  }
  int log_pc_max = DEBUG ? 10 : 1000000;
  while (log_pc_max--) {
    readFileData();
  }
  fclose(log_file);
  double acc = (double)btb_hit / bp_cnt;
  printf("[version btb]     branch_cnt= %lu btb_hit = %lu ACC = %.3f%%\n",
         bp_cnt, btb_hit, acc * 100);
  return 0;
}
