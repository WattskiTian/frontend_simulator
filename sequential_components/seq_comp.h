#ifndef _SEQ_COMP_H_
#define _SEQ_COMP_H_

#include <cstdint>

void print_all_seq_components();

// the defination of all sequential components

// dir predictor
#include "../BPU/dir_predictor/config_dir.h"
// tage
extern uint32_t u_clear_cnt;
extern uint32_t FH[FH_N_MAX][TN_MAX];
extern bool GHR[GHR_LENGTH];
extern uint8_t base_counter[BASE_ENTRY_NUM];
extern uint8_t tag_table[TN_MAX][TN_ENTRY_NUM];
extern uint8_t cnt_table[TN_MAX][TN_ENTRY_NUM];
extern uint8_t useful_table[TN_MAX][TN_ENTRY_NUM];
extern const uint32_t ghr_length[TN_MAX];
extern const uint32_t fh_length[FH_N_MAX][TN_MAX];

// target predictor
#include "../BPU/target_predictor/config_target.h"
// btb
extern uint32_t btb_bta[BTB_WAY_NUM][BTB_ENTRY_NUM];
extern uint32_t btb_bta[BTB_WAY_NUM][BTB_ENTRY_NUM];
extern bool btb_valid[BTB_WAY_NUM][BTB_ENTRY_NUM];
extern uint8_t btb_br_type[BTB_WAY_NUM][BTB_ENTRY_NUM];
extern uint8_t btb_lru[BTB_ENTRY_NUM];
extern uint8_t btb_tag[BTB_WAY_NUM][BTB_ENTRY_NUM];
// ras
extern uint32_t ras[RAS_ENTRY_NUM];
extern uint32_t ras_cnt[RAS_ENTRY_NUM];
extern uint32_t ras_sp;
// target cache
extern uint32_t bht[BHT_ENTRY_NUM];
extern uint32_t target_cache[TC_ENTRY_NUM];

#endif
