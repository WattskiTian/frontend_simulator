#ifndef TARGET_PREDICTOR_TYPES_H
#define TARGET_PREDICTOR_TYPES_H

#include "../../../sequential_components/seq_comp.h"
#include "btb_IO.h"
#include <cstdint>

struct btb_pred1_In {
  uint32_t pc;
};

struct btb_pred1_Out {
  uint32_t btb_idx;
  uint8_t btb_tag;
};

struct btb_pred2_In {
  uint32_t pc;
  uint32_t btb_idx;
  uint8_t btb_tag;
  bool btb_valid_read[BTB_WAY_NUM];
  uint8_t btb_tag_read[BTB_WAY_NUM];
  uint8_t btb_lru_read;
  uint8_t btb_br_type_read[BTB_ENTRY_NUM];
  uint32_t btb_bta_read[BTB_ENTRY_NUM];
};

struct btb_pred2_Out {
  uint8_t btb_lru_ctrl;
  uint8_t btb_lru_wdata;
  uint32_t btb_pred_addr;
};

struct update_lru_In {
  int way;
  uint8_t btb_lru_read;
};

struct update_lru_Out {
  uint8_t btb_lru_wdata;
};

struct btb_update_In {
  uint32_t pc;
  uint32_t actualAddr;
  uint32_t br_type;
  bool actual_dir;
  uint32_t btb_idx;
  uint8_t btb_tag;
  bool btb_valid_read[BTB_WAY_NUM];
  uint8_t btb_tag_read[BTB_WAY_NUM];
  uint8_t btb_lru_read;
  uint8_t btb_br_type_read[BTB_ENTRY_NUM];
  uint32_t btb_bta_read[BTB_ENTRY_NUM];
};

struct btb_update_Out {
  uint8_t btb_valid_ctrl[BTB_WAY_NUM];
  bool btb_valid_wdata[BTB_WAY_NUM];
  uint8_t btb_tag_ctrl[BTB_WAY_NUM];
  uint8_t btb_tag_wdata[BTB_WAY_NUM];
  uint8_t btb_lru_ctrl;
  uint8_t btb_lru_wdata;
  uint8_t btb_br_type_ctrl[BTB_ENTRY_NUM];
  uint8_t btb_br_type_wdata[BTB_ENTRY_NUM];
  uint32_t btb_bta_ctrl[BTB_ENTRY_NUM];
  uint32_t btb_bta_wdata[BTB_ENTRY_NUM];
};

struct ras_push_In {
  uint32_t addr;
  uint32_t ras_read;
  uint8_t ras_cnt_read;
  uint32_t ras_sp_read;
};

struct ras_push_Out {
  uint8_t ras_cnt_ctrl;
  uint8_t ras_cnt_wdata;
  uint32_t ras_sp_ctrl;
  uint32_t ras_sp_wdata;
  uint8_t ras_ctrl;
  uint32_t ras_wdata;
};

struct ras_pop_In {
  uint32_t ras_read;
  uint8_t ras_cnt_read;
  uint32_t ras_sp_read;
};

struct ras_pop_Out {
  uint8_t ras_cnt_ctrl;
  uint8_t ras_cnt_wdata;
  uint32_t ras_sp_ctrl;
  uint32_t ras_sp_wdata;
  uint32_t ras_pop_value;
};

struct tc_pred1_In {
  uint32_t pc;
};

struct tc_pred1_Out {
  uint32_t bht_idx;
};

struct tc_pred2_In {
  uint32_t pc;
  uint32_t bht_read;
};

struct tc_pred2_Out {
  uint32_t tc_idx;
};

struct bht_update_In {
  bool pc_dir;
  uint32_t bht_read;
};

struct bht_update_Out {
  uint8_t bht_ctrl;
  uint8_t bht_wdata;
};

struct tc_update_In {
  uint32_t pc;
  uint32_t actualAddr;
  uint32_t bht_read;
};

struct tc_update_Out {
  uint8_t tc_ctrl;
  uint32_t tc_wdata;
  uint32_t tc_idx;
};
#endif
