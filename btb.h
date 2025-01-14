#ifndef BTB_H
#define BTB_H

#include <cstdint>

uint32_t btb_pred(uint32_t pc);
void btb_update(uint32_t pc, uint32_t actualAddr, uint32_t br_type,
                bool actualdir);
void bht_update(uint32_t pc, bool actualdir);

extern uint64_t dir_cnt;
extern uint64_t call_cnt;
extern uint64_t ret_cnt;
extern uint64_t indir_cnt;

#define BR_DIRECT 0
#define BR_CALL 1
#define BR_RET 2
#define BR_IDIRECT 3

#endif // BTB_H