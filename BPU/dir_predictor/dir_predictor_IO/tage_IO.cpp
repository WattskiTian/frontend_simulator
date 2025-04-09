#include "../../../sequential_components/seq_comp.h"
#include "../config_dir.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

/*#define DEBUG true*/
#define DEBUG false

#include "tage_types.h"

void TAGE_update_HR(HR_IO *IO) {
  // update GHR
  for (int i = GHR_LENGTH - 1; i > 0; i--) {
    IO->GHR_new[i] = IO->GHR_old[i - 1];
  }
  IO->GHR_new[0] = IO->new_history;

  for (int k = 0; k < FH_N_MAX; k++) {
    for (int i = 0; i < TN_MAX; i++) {
      bool old_highest;
      old_highest = (IO->FH_old[k][i] >> (fh_length[k][i] - 1)) & (0x1);
      IO->FH_new[k][i] =
          (IO->FH_old[k][i] << 1) & ((0x1 << fh_length[k][i]) - 1);
      IO->FH_new[k][i] |= IO->new_history ^ old_highest;
      IO->FH_new[k][i] ^= IO->GHR_new[ghr_length[i]]
                          << (ghr_length[i] % fh_length[k][i]);
    }
  }
}

uint8_t tage_cal_tag(uint32_t FH1, uint32_t FH2, uint32_t PC) {
  return (FH1 ^ FH2 ^ (PC >> 2)) & TAGE_TAG_MASK;
}

uint32_t tage_cal_index(uint32_t FH0, uint32_t PC) {
  return (FH0 ^ (PC >> 2)) & TAGE_INDEX_MASK;
}

void TAGE_pred_1(pred_1_IO *IO) {
  uint32_t PC = IO->pc;
  IO->base_idx = PC % BASE_ENTRY_NUM; // PC[x:0]
  for (int i = 0; i < TN_MAX; i++) {
    IO->tag_pc[i] = tage_cal_tag(IO->FH[1][i], IO->FH[2][i], PC);
  }
  for (int i = 0; i < TN_MAX; i++) {
    IO->index[i] = tage_cal_index(IO->FH[0][i], PC);
  }
}

void TAGE_pred_2(pred_2_IO *IO) {
  uint8_t pcpn = TN_MAX;
  uint8_t altpcpn = TN_MAX;
  // Take the longest history entry
  for (int i = TN_MAX - 1; i >= 0; i--) {
    if (IO->tag_read[i] == IO->tag_pc[i]) {
      pcpn = i;
      break;
    }
  }
  // get the altpcpn info for updating policies
  for (int i = pcpn - 1; i >= 0; i--) {
    if (IO->tag_read[i] == IO->tag_pc[i]) {
      altpcpn = i;
      break;
    }
  }
  bool base_pred = IO->base_cnt >= 2 ? true : false;
  if (altpcpn >= TN_MAX) { // alt not found
    IO->altpred = base_pred;
  } else {
    if (IO->cnt_read[altpcpn] >= 4) {
      IO->altpred = true;
    } else {
      IO->altpred = false;
    }
  }
  if (pcpn >= TN_MAX) { // pcpn not found
    IO->pred = base_pred;
  } else if (IO->cnt_read[pcpn] >= 4) {
    IO->pred = true;
  } else
    IO->pred = false;

  IO->pcpn = pcpn;
  IO->altpcpn = altpcpn;
}

uint8_t bit_update_2(uint8_t data, bool is_inc) {
  uint8_t ret;
  if (is_inc) {
    if (data >= 3)
      ret = 3;
    else
      ret = data + 1;

  } else {
    if (data == 0)
      ret = 0;
    else
      ret = data - 1;
  }
  return ret;
}

uint8_t bit_update_3(uint8_t data, bool is_inc) {
  uint8_t ret;
  if (is_inc) {
    if (data >= 7)
      ret = 7;
    else
      ret = data + 1;

  } else {
    if (data == 0)
      ret = 0;
    else
      ret = data - 1;
  }
  return ret;
}

// ctrl : remain/inc/dec/alloc : 0123
void TAGE_do_update(update_IO *IO) {

  uint8_t pcpn = IO->pcpn;
  uint8_t altpcpn = IO->altpcpn;
  bool alt_pred = IO->alt_pred;
  bool pred_dir = IO->pred_dir;
  bool real_dir = IO->real_dir;
  // init all ctrl to prevent previous impact!
  for (int i = 0; i < TN_MAX; i++) {
    IO->useful_ctrl[i] = 0;
    IO->cnt_ctrl[i] = 0;
    IO->tag_ctrl[i] = 0;
    IO->base_ctrl = 0;
    IO->u_clear_ctrl = 0;

    // for training: all to 0
    IO->useful_wdata[i] = 0;
    IO->cnt_wdata[i] = 0;
    IO->tag_wdata[i] = 0;
    IO->base_wdata = 0;
    IO->u_clear_cnt_wdata = 0;
  }
  // 1. update 2-bit useful counter
  // pcpn found
  if (pcpn < TN_MAX) {
    if ((pred_dir != alt_pred)) {
      if (pred_dir == real_dir) {
        IO->useful_ctrl[pcpn] = 1;
        IO->useful_wdata[pcpn] = bit_update_2(IO->useful_read[pcpn], true);
      } else {
        IO->useful_ctrl[pcpn] = 2;
        IO->useful_wdata[pcpn] = bit_update_2(IO->useful_read[pcpn], false);
      }
    }

    // 2. update cnt
    if (real_dir == true) {
      IO->cnt_ctrl[pcpn] = 1;
      IO->cnt_wdata[pcpn] = bit_update_3(IO->cnt_read[pcpn], true);
    } else {
      IO->cnt_ctrl[pcpn] = 2;
      IO->cnt_wdata[pcpn] = bit_update_3(IO->cnt_read[pcpn], false);
    }
  }
  // pcpn not found, update base_counter
  else {
    if (real_dir == true) {
      IO->base_ctrl = 1;
      IO->base_wdata = bit_update_2(IO->base_read, true);
    } else {
      IO->base_ctrl = 2;
      IO->base_wdata = bit_update_2(IO->base_read, false);
    }
  }

  // 3. pred_dir != real_dir
  // If the provider component Ti is not the component using
  // the longest history (i.e., i < M) , we try to allocate an entry on a
  // predictor component Tk using a longer history than Ti (i.e., i < k < M)

  if (pred_dir != real_dir) {

    bool new_entry_found_j = false;
    int j_i;
    bool new_entry_found_k = false;
    int k_i;

    if (pcpn <= TN_MAX - 2 ||
        pcpn == TN_MAX) { // pcpn is NOT using the longest history or not found

      for (int i = pcpn == TN_MAX ? 0 : (pcpn + 1); i < TN_MAX; i++) {
        // try to find a useful==0
        uint8_t now_useful_i =
            IO->useful_ctrl[i] ? IO->useful_wdata[i] : IO->useful_read[i];
        if (now_useful_i == 0) {
          if (new_entry_found_j == false) {
            new_entry_found_j = true;
            j_i = i;
            continue;
          } else {
            new_entry_found_k = true;
            k_i = i;
            break;
          }
        }
      }
      if (new_entry_found_j == false) { // no new entry allocated
        for (int i = pcpn + 1; i < TN_MAX; i++) {
          if (IO->useful_ctrl[i] != 0) {
            IO->useful_wdata[i] = bit_update_2(
                IO->useful_wdata[i], false); // might already be updated !
          } else {
            IO->useful_wdata[i] = bit_update_2(IO->useful_read[i], false);
          }
          IO->useful_ctrl[i] = 2;
        }
      }
      // alocate new entry
      else {

        int random_pick = IO->lsfr % 3;
        if (new_entry_found_k == true && random_pick == 0) {
          IO->tag_ctrl[k_i] = 3;
          IO->tag_wdata[k_i] = IO->tag_pc[k_i];
          IO->cnt_ctrl[k_i] = 3;
          IO->cnt_wdata[k_i] = real_dir ? 4 : 3;
          IO->useful_ctrl[k_i] = 3;
          IO->useful_wdata[k_i] = 0;
        } else {
          IO->tag_ctrl[j_i] = 3;
          IO->tag_wdata[j_i] = IO->tag_pc[j_i];
          IO->cnt_ctrl[j_i] = 3;
          IO->cnt_wdata[j_i] = real_dir ? 4 : 3;
          IO->useful_ctrl[j_i] = 3;
          IO->useful_wdata[j_i] = 0;
        }
      }
    }
  }

  // 4. Periodically, the whole column of
  // most significant bits of the u counters is reset to zero, then whole column
  // of least significant bits are reset.
  uint32_t u_clear_cnt = IO->u_clear_cnt_read + 1;
  uint32_t u_cnt = u_clear_cnt & U_CNT_MASK;
  uint32_t row_cnt = (u_clear_cnt >> U_CNT_LEN) & TAGE_INDEX_MASK;
  bool u_msb_reset = ((u_clear_cnt) >> U_MSB_OFFSET) & HIGH_MASK;

  /*IO->u_clear_cnt_wen = 1;*/
  IO->u_clear_cnt_wdata = u_clear_cnt;
  if (u_cnt == 0) {
    IO->u_clear_ctrl = LOW_MASK;     // 0b10, do reset
    IO->u_clear_ctrl |= u_msb_reset; // msb or lsb
    IO->u_clear_idx = row_cnt;
  }
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#include "./tage_IO.h"

pred_out C_TAGE_do_pred_wrapper(uint32_t pc) {

  pred_1_IO *pred_IO1 = new pred_1_IO();
  pred_IO1->pc = pc;
  for (int k = 0; k < FH_N_MAX; k++) {
    for (int i = 0; i < TN_MAX; i++) {
      pred_IO1->FH[k][i] = FH[k][i];
    }
  }

  TAGE_pred_1(pred_IO1);

  pred_2_IO *pred_IO2 = new pred_2_IO();
  pred_IO2->base_cnt = base_counter[pred_IO1->base_idx];
  for (int i = 0; i < TN_MAX; i++) {
    pred_IO2->cnt_read[i] = cnt_table[i][pred_IO1->index[i]];
    pred_IO2->tag_read[i] = tag_table[i][pred_IO1->index[i]];
    pred_IO2->tag_pc[i] = pred_IO1->tag_pc[i];
  }

  TAGE_pred_2(pred_IO2);

  pred_out pred_out;
  pred_out.pred = pred_IO2->pred;
  pred_out.altpred = pred_IO2->altpred;
  pred_out.pcpn = pred_IO2->pcpn;
  pred_out.altpcpn = pred_IO2->altpcpn;
  delete pred_IO1;
  delete pred_IO2;
  return pred_out;
}

void C_TAGE_do_update_wrapper(uint32_t pc, bool real_dir, pred_out pred_out) {
  update_IO *upd_IO = new update_IO();
  // prepare Input
  upd_IO->pc = pc;
  upd_IO->real_dir = real_dir;
  upd_IO->pred_dir = pred_out.pred;
  upd_IO->alt_pred = pred_out.altpred;
  upd_IO->pcpn = pred_out.pcpn;
  upd_IO->altpcpn = pred_out.altpcpn;

  // now FH is not updated yet!
  uint32_t index[TN_MAX];
  for (int i = 0; i < TN_MAX; i++) {
    index[i] = tage_cal_index(FH[0][i], pc);
  }
  for (int i = 0; i < TN_MAX; i++) {
    assert(index[i] < TN_ENTRY_NUM);
    upd_IO->useful_read[i] = useful_table[i][index[i]];
    upd_IO->cnt_read[i] = cnt_table[i][index[i]];
  }
  uint32_t base_idx = pc % BASE_ENTRY_NUM;
  upd_IO->base_read = base_counter[base_idx];
  upd_IO->lsfr = random();

  // now FH is not updated yet!
  uint32_t tag_pc[TN_MAX];
  for (int i = 0; i < TN_MAX; i++) {
    tag_pc[i] = tage_cal_tag(FH[1][i], FH[2][i], pc);
  }
  for (int i = 0; i < TN_MAX; i++) {
    upd_IO->tag_pc[i] = tag_pc[i];
  }
  upd_IO->u_clear_cnt_read = u_clear_cnt;

  // do the logic
  TAGE_do_update(upd_IO);

  // access sequential components
  for (int i = 0; i < TN_MAX; i++) {
    if (upd_IO->useful_ctrl[i] != 0) {
      useful_table[i][index[i]] = upd_IO->useful_wdata[i];
    }
    if (upd_IO->cnt_ctrl[i] != 0) {
      cnt_table[i][index[i]] = upd_IO->cnt_wdata[i];
    }
    if (upd_IO->tag_ctrl[i] != 0) {
      tag_table[i][index[i]] = upd_IO->tag_wdata[i];
    }
  }
  if (upd_IO->base_ctrl != 0) {
    base_counter[base_idx] = upd_IO->base_wdata;
  }
  if (upd_IO->u_clear_ctrl != 0) {
    if ((upd_IO->u_clear_ctrl & 0x1) == 1) {
      for (int i = 0; i < TN_MAX; i++) {
        useful_table[i][upd_IO->u_clear_idx] &= HIGH_MASK;
      }
    } else {
      for (int i = 0; i < TN_MAX; i++) {
        useful_table[i][upd_IO->u_clear_idx] &= LOW_MASK;
      }
    }
  }
  /*if (upd_IO->u_clear_cnt_wen) {*/
  u_clear_cnt = upd_IO->u_clear_cnt_wdata;
  delete upd_IO;
  /*}*/
}

void C_TAGE_update_HR_wrapper(bool new_history) {
  HR_IO *hr_IO = new HR_IO();
  // update History regs
  for (int k = 0; k < FH_N_MAX; k++) {
    for (int i = 0; i < TN_MAX; i++) {
      hr_IO->FH_old[k][i] = FH[k][i];
    }
  }
  for (int i = 0; i < GHR_LENGTH; i++) {
    hr_IO->GHR_old[i] = GHR[i];
  }
  hr_IO->new_history = new_history;

  // do the logic
  TAGE_update_HR(hr_IO);

  // access sequential components
  for (int k = 0; k < FH_N_MAX; k++) {
    for (int i = 0; i < TN_MAX; i++) {
      FH[k][i] = hr_IO->FH_new[k][i];
    }
  }
  for (int i = 0; i < GHR_LENGTH; i++) {
    GHR[i] = hr_IO->GHR_new[i];
  }
  delete hr_IO;
}