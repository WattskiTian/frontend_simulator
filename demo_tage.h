#ifndef DEMO_TAGE_H
#define DEMO_TAGE_H

#include <cstdint>

bool TAGE_get_prediction(uint32_t PC);
void TAGE_do_update(uint32_t PC, bool real_dir, bool pred_dir);

#endif // DEMO_TAGE_H