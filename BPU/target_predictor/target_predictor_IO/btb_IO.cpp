#include <cstdint>
#include <cstdio>
#include <sys/types.h>

#include "../../../frontend.h"
#include "../../../sequential_components/seq_comp.h"
#include "btb_IO.h"
#include "ras_IO.h"
#include "target_cache_IO.h"
#include "target_predictor_types.h"

uint32_t btb_get_tag(uint32_t pc) { return (pc >> BTB_IDX_LEN) & BTB_TAG_MASK; }

uint32_t btb_get_idx(uint32_t pc) { return pc & BTB_IDX_MASK; }

// only for statistic
// uint64_t dir_cnt = 0;
// uint64_t call_cnt = 0;
// uint64_t ret_cnt = 0;
// uint64_t indir_cnt = 0;
void update_lru(uint32_t idx, int way) {
  uint32_t current_age = (btb_lru[idx] >> (way * 2)) & 0x3;

  // update all younger ways, but not exceed 3
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    if (i == way)
      continue;
    uint32_t age = (btb_lru[idx] >> (i * 2)) & 0x3;
    if (age <= current_age) {
      uint32_t new_age = (age == 3 ? 3 : age + 1) & 0x3;
      // clear current age
      btb_lru[idx] &= ~(0x3 << (i * 2));
      // set new age
      btb_lru[idx] |= (new_age << (i * 2));
    }
  }

  // set current way to latest
  btb_lru[idx] &= ~(0x3 << (way * 2));
}

void btb_pred1(struct btb_pred1_In *in, struct btb_pred1_Out *out) {
  out->btb_idx = btb_get_idx(in->pc);
  out->btb_tag = btb_get_tag(in->pc);
}

void btb_pred2(struct btb_pred2_In *in, struct btb_pred2_Out *out) {
  // reset all outputs to prevent previous values
  out->btb_lru_ctrl = 0;
  out->btb_lru_wdata = 0;
  out->btb_pred_addr = 0;

  DEBUG_LOG("btb_pred2: pc: %x, btb_idx: %x, btb_tag: %x\n", in->pc,
            in->btb_idx, in->btb_tag);
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    DEBUG_LOG("btb_pred2: btb_valid_read[%d]: %x, btb_tag_read[%d]: %x, "
              "btb_br_type_read[%d]: %x, btb_bta_read[%d]: %x\n",
              i, in->btb_valid_read[i], i, in->btb_tag_read[i], i,
              in->btb_br_type_read[i], i, in->btb_bta_read[i]);
  }
  // find match in all ways
  for (int way = 0; way < BTB_WAY_NUM; way++) {
    if (in->btb_valid_read[way] && in->btb_tag_read[way] == in->btb_tag) {
      // // update LRU
      out->btb_lru_ctrl = 3; // alloc
      uint32_t current_age = (in->btb_lru_read >> (way * 2)) & 0x3;

      // update all older ways, but not exceed 3
      for (int i = 0; i < BTB_WAY_NUM; i++) {
        if (i == way)
          continue;
        uint32_t age = (in->btb_lru_read >> (i * 2)) & 0x3;
        if (age <= current_age) {
          uint32_t new_age = (age == 3 ? 3 : age + 1) & 0x3;
          out->btb_lru_wdata = in->btb_lru_read & (~(0x3 << (i * 2)));
          out->btb_lru_wdata |= (new_age << (i * 2));
        }
      }
      // set current way to latest
      out->btb_lru_wdata &= ~(0x3 << (way * 2));
      // update_lru(in->btb_idx, way);

      uint8_t br_type = in->btb_br_type_read[way];
      if (br_type == BR_DIRECT) {
        // dir_cnt++;
        out->btb_pred_addr = in->btb_bta_read[way];
        return;
      } else if (br_type == BR_CALL) {
        // call_cnt++;
        // ras_push(in->pc + 4);
        C_ras_push_wrapper(in->pc + 4);
        out->btb_pred_addr = in->btb_bta_read[way];
        return;
      } else if (br_type == BR_RET) {
        // ret_cnt++;
        // out->btb_pred_addr = ras_pop();
        out->btb_pred_addr = C_ras_pop_wrapper();
        return;
      } else {
        // indir_cnt++;
        // out->btb_pred_addr = tc_pred(in->pc);
        out->btb_pred_addr = C_tc_pred_wrapper(in->pc);
        return;
      }
    }
  }
  DEBUG_LOG("[btb_pred] btb miss");
  out->btb_pred_addr = in->pc + 4; // btb miss
  return;
}

void btb_update(uint32_t pc, uint32_t actualAddr, uint32_t br_type,
                bool actualdir) {
  DEBUG_LOG("[btb_update] pc: %x, actualAddr: %x, br_type: %x, actualdir: %d\n",
            pc, actualAddr, br_type, actualdir);
  uint32_t idx = btb_get_idx(pc);
  uint32_t tag = btb_get_tag(pc);
  // find match in all ways
  for (int way = 0; way < BTB_WAY_NUM; way++) {
    if (btb_valid[way][idx] && btb_tag[way][idx] == tag) {
      btb_bta[way][idx] = actualAddr;
      btb_br_type[way][idx] = br_type;

      update_lru(idx, way);

      if (br_type == BR_IDIRECT) {
        tc_update(pc, actualAddr);
      }
      return;
    }
  }

  // find empty way
  for (int way = 0; way < BTB_WAY_NUM; way++) {
    if (!btb_valid[way][idx]) {
      btb_valid[way][idx] = true;
      btb_tag[way][idx] = tag;
      btb_bta[way][idx] = actualAddr;
      btb_br_type[way][idx] = br_type;
      update_lru(idx, way);
      DEBUG_LOG("[btb_update] btb allocated\n");
      if (br_type == BR_IDIRECT) {
        tc_update(pc, actualAddr);
      }
      return;
    }
  }

  // all ways are occupied, find LRU way to replace
  int lru_way = 0;
  uint32_t max_age = 0;
  for (int way = 0; way < BTB_WAY_NUM; way++) {
    uint32_t age = (btb_lru[idx] >> (way * 2)) & 0x3;
    if (age > max_age) {
      max_age = age;
      lru_way = way;
    }
  }

  // replace LRU way
  btb_valid[lru_way][idx] = true;
  btb_tag[lru_way][idx] = tag;
  btb_bta[lru_way][idx] = actualAddr;
  btb_br_type[lru_way][idx] = br_type;
  update_lru(idx, lru_way);

  if (br_type == BR_IDIRECT) {
    tc_update(pc, actualAddr);
  }
}

void btb_update_IO(struct btb_update_In *in, struct btb_update_Out *out) {
  // reset all outputs to prevent previous values
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    out->btb_valid_ctrl[i] = 0;
    out->btb_valid_wdata[i] = 0;
    out->btb_tag_ctrl[i] = 0;
    out->btb_tag_wdata[i] = 0;
    out->btb_lru_ctrl = 0;
    out->btb_lru_wdata = 0;
    out->btb_br_type_ctrl[i] = 0;
    out->btb_br_type_wdata[i] = 0;
    out->btb_bta_ctrl[i] = 0;
    out->btb_bta_wdata[i] = 0;
  }

  // find match in all ways
  for (int way = 0; way < BTB_WAY_NUM; way++) {
    if (in->btb_valid_read[way] && in->btb_tag_read[way] == in->btb_tag) {
      out->btb_bta_ctrl[way] = 3;
      out->btb_bta_wdata[way] = in->actualAddr;
      out->btb_br_type_ctrl[way] = 3;
      out->btb_br_type_wdata[way] = in->br_type;

      update_lru(in->btb_idx, way);

      if (in->br_type == BR_IDIRECT) {
        // tc_update(in->pc, in->actualAddr);
        C_tc_update_wrapper(in->pc, in->actualAddr);
      }
      return;
    }
  }

  // find empty way
  for (int way = 0; way < BTB_WAY_NUM; way++) {
    if (!in->btb_valid_read[way]) {
      out->btb_valid_ctrl[way] = 3;
      out->btb_valid_wdata[way] = true;
      out->btb_tag_ctrl[way] = 3;
      out->btb_tag_wdata[way] = in->btb_tag;
      out->btb_bta_ctrl[way] = 3;
      out->btb_bta_wdata[way] = in->actualAddr;
      out->btb_br_type_ctrl[way] = 3;
      out->btb_br_type_wdata[way] = in->br_type;

      update_lru(in->btb_idx, way);

      if (in->br_type == BR_IDIRECT) {
        // tc_update(in->pc, in->actualAddr);
        C_tc_update_wrapper(in->pc, in->actualAddr);
      }
      return;
    }
  }

  // all ways are occupied, find LRU way to replace
  int lru_way = 0;
  uint32_t max_age = 0;
  for (int way = 0; way < BTB_WAY_NUM; way++) {
    uint32_t age = (in->btb_lru_read >> (way * 2)) & 0x3;
    if (age > max_age) {
      max_age = age;
      lru_way = way;
    }
  }

  // replace LRU way
  out->btb_valid_ctrl[lru_way] = 3;
  out->btb_valid_wdata[lru_way] = true;
  out->btb_tag_ctrl[lru_way] = 3;
  out->btb_tag_wdata[lru_way] = in->btb_tag;
  out->btb_bta_ctrl[lru_way] = 3;
  out->btb_bta_wdata[lru_way] = in->actualAddr;
  out->btb_br_type_ctrl[lru_way] = 3;
  out->btb_br_type_wdata[lru_way] = in->br_type;

  update_lru(in->btb_idx, lru_way);

  if (in->br_type == BR_IDIRECT) {
    // tc_update(in->pc, in->actualAddr);
    C_tc_update_wrapper(in->pc, in->actualAddr);
  }
}

struct btb_update_In btb_update_in;
struct btb_update_Out btb_update_out;
void C_btb_update_wrapper(uint32_t pc, uint32_t actualAddr, uint32_t br_type,
                          bool actualdir) {
  struct btb_update_In *in = &btb_update_in;
  struct btb_update_Out *out = &btb_update_out;
  in->pc = pc;
  in->actualAddr = actualAddr;
  in->actual_dir = actualdir;
  in->br_type = br_type;
  in->btb_idx = btb_get_idx(pc);
  in->btb_tag = btb_get_tag(pc);
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    in->btb_valid_read[i] = btb_valid[i][in->btb_idx];
    in->btb_tag_read[i] = btb_tag[i][in->btb_idx];
    in->btb_br_type_read[i] = btb_br_type[i][in->btb_idx];
    in->btb_bta_read[i] = btb_bta[i][in->btb_idx];
  }
  in->btb_lru_read = btb_lru[in->btb_idx];
  btb_update_IO(in, out);
  // update registers
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    if (out->btb_valid_ctrl[i] != 0) {
      btb_valid[i][in->btb_idx] = out->btb_valid_wdata[i];
    }
    if (out->btb_tag_ctrl[i] != 0) {
      btb_tag[i][in->btb_idx] = out->btb_tag_wdata[i];
    }
    if (out->btb_br_type_ctrl[i] != 0) {
      btb_br_type[i][in->btb_idx] = out->btb_br_type_wdata[i];
    }
    if (out->btb_bta_ctrl[i] != 0) {
      btb_bta[i][in->btb_idx] = out->btb_bta_wdata[i];
    }
  }
  if (out->btb_lru_ctrl != 0) {
    btb_lru[in->btb_idx] = out->btb_lru_wdata;
  }
}

struct btb_pred1_In btb_pred1_in;
struct btb_pred1_Out btb_pred1_out;
struct btb_pred2_In btb_pred2_in;
struct btb_pred2_Out btb_pred2_out;

uint32_t C_btb_pred_wrapper(uint32_t pc) {
  struct btb_pred1_In *in1 = &btb_pred1_in;
  in1->pc = pc;
  struct btb_pred1_Out *out1 = &btb_pred1_out;
  btb_pred1(in1, out1);
  uint32_t btb_idx = out1->btb_idx;
  struct btb_pred2_In *in2 = &btb_pred2_in;
  struct btb_pred2_Out *out2 = &btb_pred2_out;
  in2->pc = pc;
  in2->btb_idx = btb_idx;
  in2->btb_tag = out1->btb_tag;
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    in2->btb_valid_read[i] = btb_valid[i][btb_idx];
    in2->btb_tag_read[i] = btb_tag[i][btb_idx];
    in2->btb_br_type_read[i] = btb_br_type[i][btb_idx];
    in2->btb_bta_read[i] = btb_bta[i][btb_idx];
  }
  in2->btb_lru_read = btb_lru[btb_idx];

  // #ifdef IO_GEN_MODE
  //   if (io_gen_cnt >= 0) {
  //     printf("btb");
  //     for (int i = 0; i < BTB_WAY_NUM; i++) {
  //       printf("%d ", btb_valid[i][btb_idx]);
  //       printf("%d ", btb_tag[i][btb_idx]);
  //       printf("%d ", btb_br_type[i][btb_idx]);
  //       printf("%d ", btb_bta[i][btb_idx]);
  //     }
  //     printf("%d ", btb_lru[btb_idx]);
  //     printf("\n");
  //     printf("%d %d %d\n", ras_sp, ras[ras_sp], ras_cnt[ras_sp]);
  //   }
  // #endif

  btb_pred2(in2, out2);
  uint32_t pred_npc = out2->btb_pred_addr;
  // update regs
  if (out2->btb_lru_ctrl != 0) {
    btb_lru[btb_idx] = out2->btb_lru_wdata;
  }
  return pred_npc;
}
// using namespace std;

// // file data
// FILE *log_file;
// bool log_dir;
// uint32_t log_pc;
// uint32_t log_nextpc;
// uint32_t log_br_type;
// bool show_details = false;

// uint64_t line_cnt = 0;
// int readFileData() {
//   uint32_t num1, num2, num3, num4;
//   if (fscanf(log_file, "%u %x %x %u\n", &num1, &num2, &num3, &num4) == 4) {
//     /*printf("%u 0x%08x 0x%08x %u\n", num1, num2, num3, num4);*/
//     line_cnt++;
//     log_dir = (bool)num1;
//     log_pc = num2;
//     log_nextpc = num3;
//     log_br_type = num4;
//     /*printf("%u 0x%08x 0x%08x %u\n", log_dir, log_pc, log_nextpc,
//      * log_br_type);*/
//     return 0;
//   } else {
//     printf("log file END at line %lu\n", line_cnt);
//     return 1;
//   }
// }

// #define DEBUG false
// uint64_t control_cnt = 0;
// uint64_t btb_hit = 0;
// uint64_t dir_hit = 0;
// uint64_t ras_hit = 0;
// uint64_t call_hit = 0;
// uint64_t ret_hit = 0;
// uint64_t indir_hit = 0;

// int main() {
//   log_file = fopen("/home/watts/dhrystone/gem5output_rv/fronted_log", "r");
//   if (log_file == NULL) {
//     printf("log_file open error\n");
//     return 0;
//   }
//   int log_pc_max = DEBUG ? 10 : 1000000;
//   while (log_pc_max--) {
//     int log_eof = readFileData();
//     if (log_eof != 0)
//       break;

//     if (log_dir != 1)
//       continue; // not a control inst, need to coop with tage

//     control_cnt++;
//     uint32_t pred_npc = btb_pred(log_pc);
//     if (pred_npc == log_nextpc) {
//       btb_hit++;
//       if (log_br_type == BR_DIRECT) {
//         dir_hit++;
//       } else if (log_br_type == BR_CALL) {
//         call_hit++;
//       } else if (log_br_type == BR_RET) {
//         ret_hit++;
//       } else if (log_br_type == BR_IDIRECT) {
//         indir_hit++;
//       }
//       bht_update(log_pc, log_dir);
//       continue;
//     } else {
//       btb_update(log_pc, log_nextpc, log_br_type, log_dir);
//       bht_update(log_pc, log_dir);
//     }
//   }
//   fclose(log_file);

//   ras_hit = call_hit + ret_hit;
//   double btb_acc = (double)btb_hit / control_cnt;
//   printf("[version btb]   branch_cnt = %8lu  hit = %8lu  ACC = %6.3f%%\n",
//          control_cnt, btb_hit, btb_acc * 100);
//   double dir_acc = (double)dir_hit / dir_cnt;
//   printf("[version btb]      dir_cnt = %8lu  hit = %8lu  ACC = %6.3f%%\n",
//          dir_cnt, dir_hit, dir_acc * 100);
//   double call_acc = (double)call_hit / call_cnt;
//   printf("[version btb]     call_cnt = %8lu  hit = %8lu  ACC = %6.3f%%\n",
//          call_cnt, call_hit, call_acc * 100);
//   double ret_acc = (double)ret_hit / ret_cnt;
//   printf("[version btb]      ret_cnt = %8lu  hit = %8lu  ACC = %6.3f%%\n",
//          ret_cnt, ret_hit, ret_acc * 100);
//   double ras_acc = (double)ras_hit / (call_cnt + ret_cnt);
//   printf("[version btb]      ras_cnt = %8lu  hit = %8lu  ACC = %6.3f%%\n",
//          ret_cnt + call_cnt, ras_hit, ras_acc * 100);
//   double indir_acc = (double)indir_hit / indir_cnt;
//   printf("[version btb]    indir_cnt = %8lu  hit = %8lu  ACC = %6.3f%%\n",
//          indir_cnt, indir_hit, indir_acc * 100);
//   return 0;
// }
