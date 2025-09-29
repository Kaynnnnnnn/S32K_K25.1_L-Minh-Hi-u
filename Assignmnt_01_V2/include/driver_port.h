
#ifndef DRIVER_PORT_H_
#define DRIVER_PORT_H_

#include <stdint.h>
#include "S32K144.h"

#define DRIVER_PORT_COUNT   (5U)

typedef enum {
    DRIVER_PORTA = 0,
    DRIVER_PORTB,
    DRIVER_PORTC,
    DRIVER_PORTD,
    DRIVER_PORTE
} Driver_PortInstance;

typedef uint8_t Driver_PortMux;
#define DRIVER_PORT_MUX_GPIO   (1U)

typedef enum {
    DRIVER_PORT_IRQ_DISABLED = 0U,
    DRIVER_PORT_IRQ_RISING_EDGE,
    DRIVER_PORT_IRQ_FAILING_EDGE,
    DRIVER_PORT_IRQ_EITHER_EDGE
} Driver_PortIrqConfig;

typedef void (*Driver_PortCallback)(Driver_PortInstance port, uint8_t pin);

void DRIVER_PORT_EnableClock(Driver_PortInstance port);
void DRIVER_PORT_PinMux(Driver_PortInstance port, uint8_t pin, Driver_PortMux mux);
void DRIVER_PORT_PullConfig(Driver_PortInstance port, uint8_t pin, uint8_t enable, uint8_t pull_up);
void DRIVER_PORT_PinInterruptConfig(Driver_PortInstance port, uint8_t pin, Driver_PortIrqConfig config);
void DRIVER_PORT_RegisterCallback(Driver_PortInstance port, Driver_PortCallback cb);

#endif
