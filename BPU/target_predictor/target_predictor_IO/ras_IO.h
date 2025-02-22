#ifndef RAS_H
#define RAS_H

#include <cstdint>

void ras_push(uint32_t addr);
uint32_t ras_pop();

void C_ras_push(uint32_t addr);

#endif // RAS_H