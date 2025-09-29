#include "driver_port.h"
#include <stddef.h>

/* Simple storage for callbacks (one per port). */
static Driver_PortCallback port_callbacks[DRIVER_PORT_COUNT] = {0};

void DRIVER_PORT_EnableClock(Driver_PortInstance port) {
    (void)port;
    /* Minimal stub: real implementation should enable PCC register for port. */
}

void DRIVER_PORT_PinMux(Driver_PortInstance port, uint8_t pin, Driver_PortMux mux) {
    (void)port; (void)pin; (void)mux;
    /* Real implementation should set PCR register; stub does nothing. */
}

void DRIVER_PORT_PullConfig(Driver_PortInstance port, uint8_t pin, uint8_t enable, uint8_t pullUp) {
    (void)port; (void)pin; (void)enable; (void)pullUp;
    /* Real implementation sets PCR pull-up/down bits. */
}

void DRIVER_PORT_PinInterruptConfig(Driver_PortInstance port, uint8_t pin, Driver_PortIrqConfig irqc) {
    (void)port; (void)pin; (void)irqc;
    /* Real implementation sets interrupt config bits in PCR. */
}

void DRIVER_PORT_RegisterCallback(Driver_PortInstance port, Driver_PortCallback cb) {
    if (port < DRIVER_PORT_COUNT) {
        port_callbacks[port] = cb;
    }
}

/* Optional helper: call from real ISR (if you implement hardware ISR) */
void DRIVER_PORT_InvokeCallback(Driver_PortInstance port, uint8_t pin_index) {
    if (port < DRIVER_PORT_COUNT && port_callbacks[port]) {
        port_callbacks[port](port, pin_index);
    }
}
