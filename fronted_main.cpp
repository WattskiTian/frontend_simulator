#include "btb.h"
#include "demo_tage.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

FILE *log_file;
bool log_dir;
uint32_t log_pc;
uint32_t log_nextpc;
uint32_t log_br_type;
bool show_details = false;
uint64_t line_cnt = 0;

int readFileData() {
  uint32_t num1, num2, num3, num4;
  if (fscanf(log_file, "%u %x %x %u\n", &num1, &num2, &num3, &num4) == 4) {
    line_cnt++;
    log_dir = (bool)num1;
    log_pc = num2;
    log_nextpc = num3;
    log_br_type = num4;
    return 0;
  } else {
    printf("log file END at line %lu\n", line_cnt);
    return 1;
  }
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
int main() {
  srand(time(0));

  log_file = fopen("/home/watts/dhrystone/gem5output_rv/fronted_log", "r");
  if (log_file == NULL) {
    printf("log_file open error\n");
    return 0;
  }

  int log_pc_max = DEBUG ? 10 : 1000000;
  while (log_pc_max--) {
    int log_eof = readFileData();
    if (log_eof != 0)
      break;

    inst_cnt++;
    if (log_dir == 1)
      control_cnt++;

    // TAGE
    bool tage_dir = TAGE_get_prediction(log_pc);
    TAGE_do_update(log_pc, log_dir, tage_dir);
    if (tage_dir == log_dir)
      tage_hit++;

    uint32_t pred_npc = log_pc + 4;
    if (tage_dir == 1)
      pred_npc = btb_pred(log_pc);

    if (pred_npc == log_nextpc && log_dir == 1) {
      btb_hit++;
      if (tage_dir == 1) {
        fronted_hit++; // btb hit and tage hit
      }
      if (log_br_type == BR_DIRECT) {
        dir_hit++;
      } else if (log_br_type == BR_CALL) {
        call_hit++;
      } else if (log_br_type == BR_RET) {
        ret_hit++;
      } else if (log_br_type == BR_IDIRECT) {
        indir_hit++;
      }
      bht_update(log_pc, log_dir);
    } else {
      // update BTB regardless of hit or not if it is a branch instruction
      if (log_br_type == BR_DIRECT || log_br_type == BR_CALL ||
          log_br_type == BR_RET || log_br_type == BR_IDIRECT) {
        btb_update(log_pc, log_nextpc, log_br_type, log_dir);
        bht_update(log_pc, log_dir);
      }
    }

    //  no branch and tage predict no branch also counts as fronted hit
    if (log_dir == 0 && tage_dir == 0) {
      fronted_hit++;
    }
  }
  fclose(log_file);

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

  return 0;
}