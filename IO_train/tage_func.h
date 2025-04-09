#ifndef _TAGE_FUNC_H_
#define _TAGE_FUNC_H_

bool *tage_get_randin_cal(bool *In, int func_id, bool *output_bits,
                          int now_trainning_bit);
bool *tage_get_input(int func_id);

#define IN1_LENGTH 416
#define IN2_LENGTH 104
#define IN3_LENGTH 216
#define IN4_LENGTH 1416

#define OUT1_BYTE_LENGTH 192
#define OUT2_BYTE_LENGTH 32
#define OUT3_BYTE_LENGTH 288
#define OUT4_BYTE_LENGTH 1408

#endif
