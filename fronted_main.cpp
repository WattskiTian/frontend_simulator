#include "btb.h"
#include "demo_tage.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "frontend.h"

FILE *log_file;
bool log_dir[FETCH_WIDTH];
uint32_t log_pc[FETCH_WIDTH];
uint32_t log_nextpc[FETCH_WIDTH];
uint32_t log_br_type[FETCH_WIDTH];
bool show_details = false;
uint64_t line_cnt = 0;

int readFileData(int fetch_offset) {
  uint32_t num1, num2, num3, num4;
  if (fscanf(log_file, "%u %x %x %u\n", &num1, &num2, &num3, &num4) == 4) {
    line_cnt++;
    log_dir[fetch_offset] = (bool)num1;
    log_pc[fetch_offset] = num2;
    log_nextpc[fetch_offset] = num3;
    log_br_type[fetch_offset] = num4;
    return 0;
  } else {
    printf("log file END at line %lu\n", line_cnt);
    return 1;
  }
}

uint32_t next_fetch_address;
int first_taken_offset = 0;
int readFileData_superscalar() {
  for (int i = 0; i < FETCH_WIDTH; i++) {
    int log_eof = readFileData(i);
    if (log_eof != 0)
      return 1;
  } // fetch one group of instructions
  next_fetch_address = log_pc[0] + 4 * FETCH_WIDTH;
  for (int i = 0; i < FETCH_WIDTH; i++) {
    if (log_dir[i] == 1) {
      next_fetch_address =
          log_nextpc[i]; // if branch, update next fetch address
      first_taken_offset = i;
      return 0;
    }
  }
  return 0;
}

// BTB statistics
uint64_t control_cnt = 0;
uint64_t btb_hit = 0;
uint64_t dir_hit = 0;
uint64_t ras_hit = 0;
uint64_t call_hit = 0;
uint64_t ret_hit = 0;
uint64_t indir_hit = 0;

// TAGE statistics
uint64_t inst_cnt = 0; // total instruction count
uint64_t tage_hit = 0; // tage hit count

uint64_t fronted_hit = 0; // fronted hit count

#define DEBUG false

void print_statistics() {
  // BTB statistics
  double btb_acc = (double)btb_hit / control_cnt;
  double dir_acc = (double)dir_hit / dir_cnt;
  double call_acc = (double)call_hit / call_cnt;
  double ret_acc = (double)ret_hit / ret_cnt;
  double indir_acc = (double)indir_hit / indir_cnt;

  printf("\n=== BTB Statistics ===\n");
  printf("Branch Total: cnt = %10lu  hit = %10lu  ACC = %7.3f%%\n", control_cnt,
         btb_hit, btb_acc * 100);
  printf("Direct:      cnt = %10lu  hit = %10lu  ACC = %7.3f%%\n", dir_cnt,
         dir_hit, dir_acc * 100);
  printf("Call:        cnt = %10lu  hit = %10lu  ACC = %7.3f%%\n", call_cnt,
         call_hit, call_acc * 100);
  printf("Return:      cnt = %10lu  hit = %10lu  ACC = %7.3f%%\n", ret_cnt,
         ret_hit, ret_acc * 100);
  printf("Indirect:    cnt = %10lu  hit = %10lu  ACC = %7.3f%%\n", indir_cnt,
         indir_hit, indir_acc * 100);

  // TAGE statistics
  double tage_acc = (double)tage_hit / inst_cnt;
  printf("\n=== TAGE Statistics ===\n");
  printf("Total:       cnt = %10lu  hit = %10lu  ACC = %7.3f%%\n", inst_cnt,
         tage_hit, tage_acc * 100);

  // Fronted statistics
  double fronted_acc = (double)fronted_hit / inst_cnt;
  printf("\n=== Fronted Statistics ===\n");
  printf("Fronted:     cnt = %10lu  hit = %10lu  ACC = %7.3f%%\n", inst_cnt,
         fronted_hit, fronted_acc * 100);
}

int main() {
  srand(time(0));

  log_file = fopen("/home/watts/dhrystone/gem5output_rv/fronted_log", "r");
  if (log_file == NULL) {
    printf("log_file open error\n");
    return 0;
  }

  int log_pc_max = DEBUG ? 10 : (5000000 / FETCH_WIDTH);
  while (log_pc_max--) {
    int log_eof = readFileData_superscalar();
    if (log_eof != 0)
      break;

    inst_cnt++;
    for (int i = 0; i < FETCH_WIDTH; i++) {
      if (log_dir[i] == 1) {
        control_cnt++;
        break; // for one fetch group, only one branch is counted
      }
    }

    // first run TAGE on all the instructions in this fetch group
    // stop when a taken branch is predicted
    int tage_pred_taken_offset = 0;
    bool tage_dir = 0;
    for (int i = 0; i < FETCH_WIDTH; i++) {
      tage_dir = TAGE_get_prediction(log_pc[i]);
      if (tage_dir == 1) {
        tage_pred_taken_offset = i;
        break;
      }
    }
    if (log_dir[tage_pred_taken_offset] == tage_dir)
      tage_hit++;
    // update TAGE
    TAGE_do_update(log_pc[tage_pred_taken_offset],
                   log_dir[tage_pred_taken_offset], tage_dir);

    uint32_t pred_npc = log_pc[0] + 4 * FETCH_WIDTH; // default next pc

    if (tage_dir == 1)
      pred_npc = btb_pred(log_pc[tage_pred_taken_offset]);

    if (pred_npc == next_fetch_address) {
      fronted_hit++;
      if (log_br_type[tage_pred_taken_offset] == BR_DIRECT) {
        dir_hit++;
        btb_hit++;
      } else if (log_br_type[tage_pred_taken_offset] == BR_CALL) {
        call_hit++;
        btb_hit++;
      } else if (log_br_type[tage_pred_taken_offset] == BR_RET) {
        ret_hit++;
        btb_hit++;
      } else if (log_br_type[tage_pred_taken_offset] == BR_IDIRECT) {
        indir_hit++;
        btb_hit++;
      }
      bht_update(log_pc[tage_pred_taken_offset],
                 log_dir[tage_pred_taken_offset]);
    } else {
      // update BTB regardless of hit or not if it is a branch instruction
      if (log_br_type[tage_pred_taken_offset] == BR_DIRECT ||
          log_br_type[tage_pred_taken_offset] == BR_CALL ||
          log_br_type[tage_pred_taken_offset] == BR_RET ||
          log_br_type[tage_pred_taken_offset] == BR_IDIRECT) {
        btb_update(log_pc[tage_pred_taken_offset],
                   log_nextpc[tage_pred_taken_offset],
                   log_br_type[tage_pred_taken_offset],
                   log_dir[tage_pred_taken_offset]);
        bht_update(log_pc[tage_pred_taken_offset],
                   log_dir[tage_pred_taken_offset]);
      }
    }
  }
  fclose(log_file);

  print_statistics();
  return 0;
}
