#include "tage_func.h"
#include "../BPU/dir_predictor/dir_predictor_IO/tage_IO.h"
#include "../BPU/dir_predictor/dir_predictor_IO/tage_types.h"
#include "IO_cvt.h"

bool Out1_buf[OUT1_BYTE_LENGTH];
bool Out2_buf[OUT2_BYTE_LENGTH];
bool Out3_buf[OUT3_BYTE_LENGTH];
bool Out4_buf[OUT4_BYTE_LENGTH];

void tage_dummy(dummy_IO *IO) {
  uint32_t PC = IO->pc;
  IO->base_idx = PC % BASE_ENTRY_NUM; // PC[x:0]
}

bool *tage_get_randin_cal(bool *In, int func_id, bool *output_bits,
                          int now_trainning_bit) {

  output_bits[0] = 0; // now only one output bit

  if (func_id == 0) { // dummy test
    dummy_IO IO_dummy;
    dummy_In In_dummy;
    dummy_Out Out_dummy;
    boolArrayToStruct(In, In_dummy);
    IO_dummy.pc = In_dummy.pc;
    tage_dummy(&IO_dummy);
    Out_dummy.base_idx = IO_dummy.base_idx;
    structToBoolArray(Out_dummy, Out1_buf);
    return Out1_buf;

  } else if (func_id == 1) {
    pred_1_IO *pred_IO1 = new pred_1_IO();
    pred_1_In *In1 = new pred_1_In();
    boolArrayToStruct(In, *In1);
    pred_IO1->pc = In1->pc;
    for (int k = 0; k < FH_N_MAX; k++) {
      for (int i = 0; i < TN_MAX; i++) {
        pred_IO1->FH[k][i] = In1->FH[k][i];
      }
    }
    TAGE_pred_1(pred_IO1);
    pred_1_Out *Out1 = new pred_1_Out();
    Out1->base_idx = pred_IO1->base_idx;
    for (int i = 0; i < TN_MAX; i++) {
      Out1->index[i] = pred_IO1->index[i];
      Out1->tag_pc[i] = pred_IO1->tag_pc[i];
    }
    bool *Out_buf = new bool[OUT1_BYTE_LENGTH];
    structToBoolArray(*Out1, Out_buf);
    output_bits[0] = Out_buf[now_trainning_bit]; // now only one output bit
    delete[] Out_buf;
    delete pred_IO1;
    delete In1;
    delete Out1;
    return output_bits;

  } else if (func_id == 2) {
    pred_2_IO *pred_IO2 = new pred_2_IO();
    pred_2_In *In2 = new pred_2_In();
    boolArrayToStruct(In, *In2);
    pred_IO2->base_cnt = In2->base_cnt;
    for (int i = 0; i < TN_MAX; i++) {
      pred_IO2->tag_pc[i] = In2->tag_pc[i];
      pred_IO2->tag_read[i] = In2->tag_read[i];
      pred_IO2->cnt_read[i] = In2->cnt_read[i];
    }
    TAGE_pred_2(pred_IO2);
    pred_2_Out *Out2 = new pred_2_Out();
    Out2->pred = pred_IO2->pred;
    Out2->altpred = pred_IO2->altpred;
    Out2->pcpn = pred_IO2->pcpn;
    Out2->altpcpn = pred_IO2->altpcpn;
    bool *Out_buf = new bool[OUT2_BYTE_LENGTH];
    structToBoolArray(*Out2, Out_buf);
    output_bits[0] = Out_buf[now_trainning_bit]; // now only one output bit
    delete[] Out_buf;
    delete pred_IO2;
    delete In2;
    delete Out2;
    return output_bits;

  } else if (func_id == 3) {
    update_IO *UPD_IO = new update_IO();
    update_In *In3 = new update_In();
    boolArrayToStruct(In, *In3);
    UPD_IO->pc = In3->pc;
    UPD_IO->real_dir = In3->real_dir;
    UPD_IO->pred_dir = In3->pred_dir;
    UPD_IO->alt_pred = In3->alt_pred;
    UPD_IO->pcpn = In3->pcpn;
    UPD_IO->altpcpn = In3->altpcpn;
    for (int i = 0; i < TN_MAX; i++) {
      UPD_IO->useful_read[i] = In3->useful_read[i];
      UPD_IO->cnt_read[i] = In3->cnt_read[i];
    }
    UPD_IO->base_read = In3->base_read;
    UPD_IO->lsfr = In3->lsfr;
    for (int i = 0; i < TN_MAX; i++) {
      UPD_IO->tag_pc[i] = In3->tag_pc[i];
    }
    UPD_IO->u_clear_cnt_read = In3->u_clear_cnt_read;
    // do the logic
    TAGE_do_update(UPD_IO);
    // No need to access sequential components!
    update_Out *Out3 = new update_Out();
    for (int i = 0; i < TN_MAX; i++) {
      Out3->useful_ctrl[i] = UPD_IO->useful_ctrl[i];
      Out3->useful_wdata[i] = UPD_IO->useful_wdata[i];
      Out3->cnt_ctrl[i] = UPD_IO->cnt_ctrl[i];
      Out3->cnt_wdata[i] = UPD_IO->cnt_wdata[i];
      Out3->tag_ctrl[i] = UPD_IO->tag_ctrl[i];
      Out3->tag_wdata[i] = UPD_IO->tag_wdata[i];
      Out3->base_ctrl = UPD_IO->base_ctrl;
      Out3->base_wdata = UPD_IO->base_wdata;
      Out3->u_clear_ctrl = UPD_IO->u_clear_ctrl;
      Out3->u_clear_idx = UPD_IO->u_clear_idx;
      Out3->u_clear_cnt_wdata = UPD_IO->u_clear_cnt_wdata;
    }
    bool *Out_buf = new bool[OUT3_BYTE_LENGTH];
    structToBoolArray(*Out3, Out_buf);
    output_bits[0] = Out_buf[now_trainning_bit]; // now only one output bit
    delete[] Out_buf;
    delete UPD_IO;
    delete In3;
    delete Out3;
    return output_bits;

  } else if (func_id == 4) {
    // printf("size of HR_out: %d\n", sizeof(HR_Out));
    HR_IO *hr_IO = new HR_IO();
    HR_In *In4 = new HR_In();
    boolArrayToStruct(In, *In4);
    for (int k = 0; k < FH_N_MAX; k++) {
      for (int i = 0; i < TN_MAX; i++) {
        hr_IO->FH_old[k][i] = In4->FH_old[k][i];
      }
    }
    for (int i = 0; i < GHR_LENGTH; i++) {
      hr_IO->GHR_old[i] = In4->GHR_old[i];
    }
    hr_IO->new_history = In4->new_history;
    // do the logic
    TAGE_update_HR(hr_IO);
    // No need to access sequential components!
    HR_Out *Out4 = new HR_Out();
    for (int k = 0; k < FH_N_MAX; k++) {
      for (int i = 0; i < TN_MAX; i++) {
        Out4->FH_new[k][i] = hr_IO->FH_new[k][i];
      }
    }
    for (int i = 0; i < GHR_LENGTH; i++) {
      Out4->GHR_new[i] = hr_IO->GHR_new[i];
    }
    bool *Out_buf = new bool[OUT4_BYTE_LENGTH];
    structToBoolArray(*Out4, Out_buf);
    output_bits[0] = Out_buf[now_trainning_bit]; // now only one output bit
    delete[] Out_buf;
    delete hr_IO;
    delete In4;
    delete Out4;
    return output_bits;
  }
  assert(0);
  return Out1_buf;
}

void randbool(int n, bool *a, int zero_length) {
  int zi = 0;
  long randint;
  int i;
  for (i = 0; i < n - zero_length; i++) {
    zi = i % 30;
    if (zi == 0) {
      randint = rand();
    }
    a[i] = bool((randint >> (zi)) % 2);
  }
  for (; i < n; i++) {
    a[i] = 0;
  }
}

void randbool2(int n, bool *a, bool *b, int zero_length) {
  int zi = 0;
  long randint;
  int i;
  for (i = 0; i < n - zero_length; i++) {
    zi = i % 30;
    if (zi == 0) {
      randint = rand();
    }
    a[i] = bool((randint >> (zi)) % 2);
    b[i] = a[i];
  }
  for (; i < n; i++) {
    a[i] = 0;
    b[i] = 0;
  }
}

void randpc(bool *pc) {
  randbool(30, pc + 2, 0);
  pc[0] = 0;
  pc[1] = 0;
}

void randFH(bool *FH) {
  int idx = 0;
  for (int k = 0; k < FH_N_MAX; k++) {
    for (int i = 0; i < TN_MAX; i++) {
      // FH[k][i]
      randbool(32, FH + idx, 32 - fh_length[k][i]);
      idx += 32;
    }
  }
}

bool In1_buf[IN1_LENGTH];
bool In2_buf[IN2_LENGTH];
bool In3_buf[IN3_LENGTH];
bool In4_buf[IN4_LENGTH];

bool *tage_get_input(int func_id) {
  if (func_id == 1) {
    randpc(In1_buf);
    randFH(In1_buf + 32);
    return In1_buf;

  } else if (func_id == 2) {
    bool lsfr;
    int idx = 0;
    randbool(8, In2_buf, 6); // base_cnt
    idx += 8;
    for (int i = 0; i < TN_MAX; i++) {
      lsfr = rand() % 2;
      if (lsfr == 1) {
        randbool2(8, In2_buf + idx, In2_buf + idx + 32, 0); // tag_pc==tag_read
        randbool(8, In2_buf + idx + 64, 5);                 // cnt_read
        idx += 8;
      } else {
        randbool(8, In2_buf + idx, 0);      // tag_pc
        randbool(8, In2_buf + idx + 32, 0); // tag_read
        randbool(8, In2_buf + idx + 64, 5); // cnt_read
        idx += 8;
      }
    }
    return In2_buf;

  } else if (func_id == 3) {
    int idx = 0;
    randpc(In3_buf);
    idx += 32;
    randbool(3, In3_buf + idx, 0);
    idx += 3;
    randbool(8, In3_buf + idx, 5); // pcpn
    idx += 8;
    randbool(8, In3_buf + idx, 5); // altpcpn
    idx += 8;
    for (int i = 0; i < TN_MAX; i++) {
      randbool(8, In3_buf + idx, 6);      // useful_read
      randbool(8, In3_buf + idx + 32, 5); // cnt_read
      idx += 8;
    }
    idx += 32;
    randbool(8, In3_buf + idx, 6); // base_read
    idx += 8;
    randbool(8, In3_buf + idx, 0); // lsfr
    idx += 8;
    for (int i = 0; i < TN_MAX; i++) {
      randbool(8, In3_buf + idx, 0); // tag_pc
      idx += 8;
    }
    randbool(32, In3_buf + idx, 0); // u_clear_cnt_read
    return In3_buf;

  } else if (func_id == 4) {
    randFH(In4_buf);
    int idx = 32 * FH_N_MAX * TN_MAX;
    randbool(GHR_LENGTH + 1, In4_buf + idx, 0); // GHR_old,new_history
    return In4_buf;
  }
  return In1_buf;
}