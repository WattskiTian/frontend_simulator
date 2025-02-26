#include "seq_comp.h"
#include <stdio.h>
// stores all the sequential components as GLOBAL variables

/////////////////////TAGE/////////////////////
uint32_t u_clear_cnt;
uint32_t FH[FH_N_MAX][TN_MAX];
bool GHR[GHR_LENGTH];
uint8_t base_counter[BASE_ENTRY_NUM];
uint8_t tag_table[TN_MAX][TN_ENTRY_NUM];
uint8_t cnt_table[TN_MAX][TN_ENTRY_NUM];
uint8_t useful_table[TN_MAX][TN_ENTRY_NUM];

const uint32_t ghr_length[TN_MAX] = {8, 13, 32, 119};
const uint32_t fh_length[FH_N_MAX][TN_MAX] = {8, 11, 11, 11, 8, 8,
                                              8, 8,  7,  7,  7, 7};
/////////////////////TAGE/////////////////////

/////////////////////BTB/////////////////////
uint8_t btb_tag[BTB_WAY_NUM][BTB_ENTRY_NUM];
uint32_t btb_bta[BTB_WAY_NUM][BTB_ENTRY_NUM];
bool btb_valid[BTB_WAY_NUM][BTB_ENTRY_NUM];
uint8_t btb_br_type[BTB_WAY_NUM][BTB_ENTRY_NUM];
uint8_t btb_lru[BTB_ENTRY_NUM];
/////////////////////BTB/////////////////////

/////////////////////RAS/////////////////////
uint32_t ras[RAS_ENTRY_NUM];
uint32_t ras_cnt[RAS_ENTRY_NUM];
uint32_t ras_sp;
/////////////////////RAS/////////////////////

/////////////////////TARGET CACHE/////////////////////
uint32_t bht[BHT_ENTRY_NUM];
uint32_t target_cache[TC_ENTRY_NUM];
/////////////////////TARGET CACHE/////////////////////

void print_all_seq_components() {
  printf("%d ", u_clear_cnt);
  for (int i = 0; i < FH_N_MAX; i++) {
    for (int j = 0; j < TN_MAX; j++) {
      printf("%d ", FH[i][j]);
    }
  }
  for (int i = 0; i < GHR_LENGTH; i++) {
    printf("%d ", GHR[i]);
  }
  for (int i = 0; i < BASE_ENTRY_NUM; i++) {
    printf("%d ", base_counter[i]);
  }
  for (int i = 0; i < TN_MAX; i++) {
    for (int j = 0; j < TN_ENTRY_NUM; j++) {
      printf("%d ", tag_table[i][j]);
    }
  }
  for (int i = 0; i < TN_MAX; i++) {
    for (int j = 0; j < TN_ENTRY_NUM; j++) {
      printf("%d ", cnt_table[i][j]);
    }
  }
  for (int i = 0; i < TN_MAX; i++) {
    for (int j = 0; j < TN_ENTRY_NUM; j++) {
      printf("%d ", useful_table[i][j]);
    }
  }
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    for (int j = 0; j < BTB_ENTRY_NUM; j++) {
      printf("%d ", btb_tag[i][j]);
    }
  }
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    for (int j = 0; j < BTB_ENTRY_NUM; j++) {
      printf("%d ", btb_bta[i][j]);
    }
  }
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    for (int j = 0; j < BTB_ENTRY_NUM; j++) {
      printf("%d ", btb_valid[i][j]);
    }
  }
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    for (int j = 0; j < BTB_ENTRY_NUM; j++) {
      printf("%d ", btb_br_type[i][j]);
    }
  }
  for (int i = 0; i < BTB_ENTRY_NUM; i++) {
    printf("%d ", btb_lru[i]);
  }
  for (int i = 0; i < RAS_ENTRY_NUM; i++) {
    printf("%d ", ras[i]);
  }
  for (int i = 0; i < RAS_ENTRY_NUM; i++) {
    printf("%d ", ras_cnt[i]);
  }
  printf("%d ", ras_sp);
  for (int i = 0; i < BHT_ENTRY_NUM; i++) {
    printf("%d ", bht[i]);
  }
  for (int i = 0; i < TC_ENTRY_NUM; i++) {
    printf("%d ", target_cache[i]);
  }
}
