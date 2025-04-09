#ifndef BTB_H
#define BTB_H

#include <cstdint>

uint32_t C_btb_pred_wrapper(uint32_t pc);
void C_btb_update_wrapper(uint32_t pc, uint32_t actualAddr, uint32_t br_type,
                          bool actualdir);

// extern uint64_t dir_cnt;
// extern uint64_t call_cnt;
// extern uint64_t ret_cnt;
// extern uint64_t indir_cnt;

#endif // BTB_H