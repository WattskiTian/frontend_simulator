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
#define RAS_IDX_LEN 6
#define RAS_ENTRY_NUM (1 << RAS_IDX_LEN)
#define RAS_IDX_MASK (RAS_ENTRY_NUM - 1)
#define RAS_CNT_LEN 8 // cnt for repeated call

// TARGET CACHE CONFIGS
#define TC_IDX_LEN 7
#define TC_ENTRY_NUM (1 << TC_IDX_LEN)
#define TC_IDX_MASK (TC_ENTRY_NUM - 1)
#define BHT_IDX_LEN 7
#define BHT_ENTRY_NUM (1 << BHT_IDX_LEN)
#define BHT_IDX_MASK (BHT_ENTRY_NUM - 1)
#define BHT_LEN 16
#define BHT_MASK ((1 << BHT_LEN) - 1)

#endif // _CONFIG_TARGET_H_
