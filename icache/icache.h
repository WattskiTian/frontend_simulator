#ifndef ICACHE_H
#define ICACHE_H

#include "../front_IO.h"
#include "../front_module.h"
#include "../frontend.h"
#include <cstdint>
#include <cstdio>
#include <random>

// Cache configuration
#define ICACHE_OFFSET_WIDTH 4
#define ICACHE_INDEX_WIDTH 6
#define ICACHE_WAY_NUM 2

// Cache address manipulation macros
#define ICACHE_TAG(addr) (addr >> (ICACHE_OFFSET_WIDTH + ICACHE_INDEX_WIDTH))
#define ICACHE_INDEX(addr)                                                     \
  ((addr >> ICACHE_OFFSET_WIDTH) & ((1 << ICACHE_INDEX_WIDTH) - 1))

// Latency configuration
#define HIT_LATENCY 3
#define MISS_LATENCY 100

// Cache state definitions
#define ICACHE_IDLE 0
#define ICACHE_WAITING 1

#endif // ICACHE_H