#ifndef _SEQ_COMP_H_
#define _SEQ_COMP_H_

#include <cstdint>

// the defination of all sequential components

/////////////////////TAGE/////////////////////
#define BASE_ENTRY_NUM 2048
/*#define GHR_LENGTH 256*/
#define GHR_LENGTH 256
#define TN_MAX 4 // 0-indexed, which means 0,1,2,3
#define TN_ENTRY_NUM 4096
#define FH_N_MAX 3 // how many different types of Folded history
/*#define USEFUL_RESET_VAL 262144 // 256K*/
#define uBitPeriod 2048

extern uint32_t u_clear_cnt;
extern uint32_t FH[FH_N_MAX][TN_MAX];
extern bool GHR[GHR_LENGTH];
extern uint8_t base_counter[BASE_ENTRY_NUM];
extern uint8_t tag_table[TN_MAX][TN_ENTRY_NUM];
extern uint8_t cnt_table[TN_MAX][TN_ENTRY_NUM];
extern uint8_t useful_table[TN_MAX][TN_ENTRY_NUM];
extern const uint32_t ghr_length[TN_MAX];
extern const uint32_t fh_length[FH_N_MAX][TN_MAX];
/////////////////////TAGE/////////////////////

/////////////////////BTB/////////////////////
#define BTB_ENTRY_NUM 2048
#define BTB_TAG_LEN 8
#define BTB_WAY_NUM 4

#define BTB_IDX_LEN 11 // log2(BTB_ENTRY_NUM)
#define BTB_IDX_MASK (BTB_ENTRY_NUM - 1)
#define BTB_TAG_MASK ((1 << BTB_TAG_LEN) - 1)

#define BR_DIRECT 0
#define BR_CALL 1
#define BR_RET 2
#define BR_IDIRECT 3

extern uint32_t btb_bta[BTB_WAY_NUM][BTB_ENTRY_NUM];
extern bool btb_valid[BTB_WAY_NUM][BTB_ENTRY_NUM];
extern uint8_t btb_br_type[BTB_WAY_NUM][BTB_ENTRY_NUM];
extern uint8_t btb_lru[BTB_ENTRY_NUM];
extern uint8_t btb_tag[BTB_WAY_NUM][BTB_ENTRY_NUM];
/////////////////////BTB/////////////////////

/////////////////////RAS/////////////////////
#define RAS_ENTRY_NUM 64
#define RAS_CNT_LEN 8 // cnt for repeated call

extern uint32_t ras[RAS_ENTRY_NUM];
extern uint32_t ras_cnt[RAS_ENTRY_NUM];
extern uint32_t ras_sp;
/////////////////////RAS/////////////////////

/////////////////////TARGET CACHE/////////////////////
#define TC_ENTRY_NUM 2048
#define BHT_ENTRY_NUM 2048
#define BHT_LEN 32

extern uint32_t bht[BHT_ENTRY_NUM];
extern uint32_t target_cache[TC_ENTRY_NUM];
/////////////////////TARGET CACHE/////////////////////

#endif
