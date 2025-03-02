#include "../frontend.h"
#include "../sequential_components/seq_comp.h"
#include <bitset>
#include <cstdint>
#include <cstdio>
#include <iostream>

void print_IO_data(uint32_t base_pc) {
  for (int i = 0; i < FETCH_WIDTH; i++) {
    uint32_t pc = base_pc + (i * 4);
    // printf("%d ", pc);
    // printf("%d ", base_counter[pc % BASE_ENTRY_NUM]);

    std::cout << std::bitset<32>(pc);
    std::cout << std::bitset<2>(base_counter[pc % BASE_ENTRY_NUM]);

    uint32_t tmp_tage_idx[TN_MAX];
    for (int i = 0; i < TN_MAX; i++) {
      tmp_tage_idx[i] = FH[0][i] ^ (pc >> 2) & (0xfff);
    }
    for (int i = 0; i < TN_MAX; i++) {
      // printf("%d ", tag_table[i][tmp_tage_idx[i]]);
      // printf("%d ", cnt_table[i][tmp_tage_idx[i]]);
      // printf("%d ", useful_table[i][tmp_tage_idx[i]]);
      std::cout << std::bitset<8>(tag_table[i][tmp_tage_idx[i]]);
      std::cout << std::bitset<3>(cnt_table[i][tmp_tage_idx[i]]);
      std::cout << std::bitset<2>(useful_table[i][tmp_tage_idx[i]]);
    }
    for (int i = 0; i < BTB_WAY_NUM; i++) {
      // printf("%d ", btb_valid[i][pc & BTB_IDX_MASK]);
      // printf("%d ", btb_tag[i][pc & BTB_IDX_MASK]);
      // printf("%d ", btb_br_type[i][pc & BTB_IDX_MASK]);
      // printf("%d ", btb_bta[i][pc & BTB_IDX_MASK]);
      std::cout << std::bitset<1>(btb_valid[i][pc & BTB_IDX_MASK]);
      std::cout << std::bitset<8>(btb_tag[i][pc & BTB_IDX_MASK]);
      std::cout << std::bitset<2>(btb_br_type[i][pc & BTB_IDX_MASK]);
      std::cout << std::bitset<32>(btb_bta[i][pc & BTB_IDX_MASK]);
    }
    std::cout << std::bitset<2>(btb_lru[pc & BTB_IDX_MASK]);
    std::cout << std::bitset<32>(bht[pc % BHT_ENTRY_NUM]);
    // printf("%d ", target_cache[(pc ^ bht[pc % BHT_ENTRY_NUM]) %
    // TC_ENTRY_NUM]);
    std::cout << std::bitset<32>(
        target_cache[(pc ^ bht[pc % BHT_ENTRY_NUM]) % TC_ENTRY_NUM]);
  }
  for (int i = 0; i < GHR_LENGTH; i++) {
    // printf("%d ", GHR[i]);
    std::cout << std::bitset<1>(GHR[i]);
  }
  // out fh
  for (int i = 0; i < FH_N_MAX; i++) {
    for (int j = 0; j < TN_MAX; j++) {
      // printf("%d ", FH[i][j]);
      std::cout << std::bitset<11>(FH[i][j]);
    }
  }
  // out ras
  std::cout << std::bitset<5>(ras_sp);
  std::cout << std::bitset<32>(ras[ras_sp % RAS_ENTRY_NUM]);
  std::cout << std::bitset<8>(ras_cnt[ras_sp % RAS_ENTRY_NUM]);
}