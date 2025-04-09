#ifndef TARGET_CACHE_H
#define TARGET_CACHE_H

#include <cstdint>

uint32_t C_tc_pred_wrapper(uint32_t pc);
void C_bht_update_wrapper(uint32_t pc, bool pc_dir);
void C_tc_update_wrapper(uint32_t pc, uint32_t actualAddr);
#endif // TARGET_CACHE_H