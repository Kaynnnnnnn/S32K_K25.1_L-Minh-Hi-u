/* nvic_custom.c - small helpers and clearer code */
#include "nvic_custom.h"
#include <stddef.h>
/* Use CMSIS intrinsics if available; fallback to asm */
#ifndef __DSB
#define __DSB() __asm volatile ("dsb")
#endif
#ifndef __ISB
#define __ISB() __asm volatile ("isb")
#endif

static inline void nvic_compute_index_bit(IRQn_Type irqn, uint32_t *idx, uint32_t *bit) {
    uint32_t i = (uint32_t)irqn;
    *idx = i >> 5U;
    *bit = 1UL << (i & 0x1FUL);
}

void NVIC_EnableIRQ(IRQn_Type IRQn) {
    if (IRQn >= 0 && (uint32_t)IRQn < (uint32_t)NUMBER_OF_INT_VECTORS) {
        uint32_t idx, bit;
        nvic_compute_index_bit(IRQn, &idx, &bit);
        NVIC->ISER[idx] = bit;
        __DSB(); __ISB();
    }
}

void NVIC_DisableIRQ(IRQn_Type IRQn) {
    if (IRQn >= 0 && (uint32_t)IRQn < (uint32_t)NUMBER_OF_INT_VECTORS) {
        uint32_t idx, bit;
        nvic_compute_index_bit(IRQn, &idx, &bit);
        NVIC->ICER[idx] = bit;
        __DSB(); __ISB();
    }
}

void NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority) {
    if (IRQn >= 0 && (uint32_t)IRQn < (uint32_t)NUMBER_OF_INT_VECTORS) {
        NVIC->IP[(uint32_t)IRQn] = priority & 0xFFU;
    }
}
