#ifndef PORT_H
#define PORT_H

#include "S32K144.h"   // để biết GPIO_Type

static inline void PORT_EnableClock(GPIO_Type *port) {
    // stub: nếu dùng thực tế phải enable clock ở SIM->SCGCx
}

static inline void PORT_SetPinMux(GPIO_Type *port, uint32_t pin, uint32_t mux) {
    // stub: cấu hình mux cho pin
}

#endif
