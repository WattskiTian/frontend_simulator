#ifndef _CONFIG_TARGET_H_
#define _CONFIG_TARGET_H_

// BTB CONFIGS
#define BTB_IDX_LEN 6
#define BTB_ENTRY_NUM (1 << BTB_IDX_LEN)
#define BTB_IDX_MASK (BTB_ENTRY_NUM - 1)
#define BTB_TAG_LEN 8
#define BTB_TAG_MASK ((1 << BTB_TAG_LEN) - 1)
#define BTB_WAY_NUM 4

#define BR_DIRECT 0
#define BR_CALL 1
#define BR_RET 2
#define BR_IDIRECT 3

// LRU CONFIGS
#define LRU_LEN 2
#define LRU_MASK ((1 << LRU_LEN) - 1)

// RAS CONFIGS
#define RAS_ENTRY_NUM 64
#define RAS_CNT_LEN 8 // cnt for repeated call

// TARGET CACHE CONFIGS
#define TC_ENTRY_NUM 128
#define BHT_ENTRY_NUM 128
#define BHT_LEN 32

#endif // _CONFIG_TARGET_H_
