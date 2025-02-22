#ifndef TARGET_CACHE_H
#define TARGET_CACHE_H

#include <cstdint>

uint32_t tc_pred(uint32_t pc);
void tc_update(uint32_t pc, uint32_t actualAddr);
void bht_update(uint32_t pc, bool pc_dir);

uint32_t C_tc_pred(uint32_t pc);
void C_bht_update(uint32_t pc, bool pc_dir);
void C_tc_update(uint32_t pc, uint32_t actualAddr);
#endif // TARGET_CACHE_H