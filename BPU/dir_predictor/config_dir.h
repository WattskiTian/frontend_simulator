#ifndef CONFIG_DIR_H
#define CONFIG_DIR_H

// TAGE CONFIGS
#define TAGE_INDEX_LEN 12
#define TN_ENTRY_NUM (1 << TAGE_INDEX_LEN)
#define TAGE_INDEX_MASK (TN_ENTRY_NUM - 1)
#define BASE_ENTRY_NUM 4096
#define GHR_LENGTH 256
#define TN_MAX 4   // 0-indexed, which means 0,1,2,3
#define FH_N_MAX 3 // how many different types of Folded history

// U COUNTER CONFIGS
#define U_CNT_LEN 11
#define U_CNT_MASK ((1 << U_CNT_LEN) - 1)
#define U_MSB_OFFSET (U_CNT_LEN + TAGE_INDEX_LEN)

#define LOW_MASK 0x2
#define HIGH_MASK 0x1

#endif // CONFIG_DIR_H