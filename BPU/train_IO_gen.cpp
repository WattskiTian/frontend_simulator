#include "../sequential_components/seq_comp.h"
#include <cstdint>
#include <cstdio>

void print_IO_data(uint32_t pc) {
  printf("%d ", pc);
  printf("%d ", base_counter[pc % BASE_ENTRY_NUM]);
  uint32_t tmp_tage_idx[TN_MAX];
  for (int i = 0; i < TN_MAX; i++) {
    tmp_tage_idx[i] = FH[0][i] ^ (pc >> 2) & (0xfff);
  }
  for (int i = 0; i < TN_MAX; i++) {
    printf("%d ", tag_table[i][tmp_tage_idx[i]]);
    printf("%d ", cnt_table[i][tmp_tage_idx[i]]);
    printf("%d ", useful_table[i][tmp_tage_idx[i]]);
  }
  for (int i = 0; i < GHR_LENGTH; i++) {
    printf("%d ", GHR[i]);
  }
  for (int i = 0; i < BTB_WAY_NUM; i++) {
    printf("%d ", btb_valid[i][pc & BTB_IDX_MASK]);
    printf("%d ", btb_tag[i][pc & BTB_IDX_MASK]);
    printf("%d ", btb_br_type[i][pc & BTB_IDX_MASK]);
    printf("%d ", btb_bta[i][pc & BTB_IDX_MASK]);
  }
  printf("%d ", btb_lru[pc & BTB_IDX_MASK]);
  printf("%d %d %d", ras_sp, ras[ras_sp % RAS_ENTRY_NUM],
         ras_cnt[ras_sp % RAS_ENTRY_NUM]);
  printf("%d ", bht[pc % BHT_ENTRY_NUM]);
  printf("%d ", target_cache[(pc ^ bht[pc % BHT_ENTRY_NUM]) % TC_ENTRY_NUM]);
}
