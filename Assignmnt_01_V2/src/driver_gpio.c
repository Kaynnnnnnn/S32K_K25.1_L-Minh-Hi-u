/* driver_gpio.c - optimized, clearer structure */
#include "driver_gpio.h"
#include "driver_port.h"
#include "nvic_custom.h"
#include <stdint.h>

#define MAX_PORTS       5U
#define PIN_NOT_MAPPED  0xFFU

/* pin_table comes from driver_gpio.h (extern const pin_desp_t pin_table[]) */
const pin_desp_t pin_table[] = {
    [BUTTON1]   = {DRIVER_PORTC, 12U},
    [BUTTON2]   = {DRIVER_PORTC, 13U},
    [LED_RED]   = {DRIVER_PORTD, 15U},
    [LED_GREEN] = {DRIVER_PORTD, 16U}
};

/* Helper: map Driver_PortInstance -> GPIO_Type* */
static inline GPIO_Type* get_gpio_base(Driver_PortInstance port) {
    switch (port) {
    case DRIVER_PORTA: return IP_PTA;
    case DRIVER_PORTB: return IP_PTB;
    case DRIVER_PORTC: return IP_PTC;
    case DRIVER_PORTD: return IP_PTD;
    case DRIVER_PORTE: return IP_PTE;
    default:           return IP_PTA;
    }
}

/* Per-port callback and per-port pin mapping:
   - port_callbacks[port] stores the single callback registered for that port (common pattern)
   - pin_map[port][pin] maps physical pin index (0..31) -> logical ARM_GPIO_Pin_t (0xFF = none)
*/
static ARM_GPIO_SignalEvent_t port_callbacks[MAX_PORTS] = {0};
static uint8_t pin_map[MAX_PORTS][PINS_PER_PORT];

static void init_pin_map(void) {
    for (uint32_t p = 0; p < MAX_PORTS; ++p) {
        for (uint32_t i = 0; i < PINS_PER_PORT; ++i) {
            pin_map[p][i] = PIN_NOT_MAPPED;
        }
    }
}

/* Called by PORT driver ISR: port index + pin index */
static void gpio_port_forward_event(Driver_PortInstance port, uint8_t pin_index) {
    if (port >= MAX_PORTS) return;
    uint8_t mapped = pin_map[port][pin_index];
    if (mapped != PIN_NOT_MAPPED && port_callbacks[port]) {
        /* We can't reliably detect rising/falling from ISFR alone.
           Forward EITHER_EDGE and let the callback read current level if needed. */
        port_callbacks[port]((ARM_GPIO_Pin_t)mapped, ARM_GPIO_EVENT_EITHER_EDGE);
    }
}

/* ================= Driver API implementations ================= */

static int32_t ARM_GPIO_Setup(ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event) {
    static uint8_t initialized = 0;
    if (!initialized) { init_pin_map(); initialized = 1; }

    if (pin >= (sizeof(pin_table)/sizeof(pin_table[0]))) return ARM_GPIO_ERROR_PIN;

    Driver_PortInstance port = pin_table[pin].port;
    uint8_t physical_pin = pin_table[pin].pin;

    if (!PIN_IS_AVAILABLE(physical_pin)) return ARM_GPIO_ERROR_PIN;

    /* Enable port clock + mux to GPIO */
    DRIVER_PORT_EnableClock(port);
    DRIVER_PORT_PinMux(port, physical_pin, DRIVER_PORT_MUX_GPIO);

    /* register callback mapping if provided */
    if (cb_event) {
        port_callbacks[port] = cb_event;
        DRIVER_PORT_RegisterCallback(port, gpio_port_forward_event);

        );
    }

    /* store mapping physical->logical pin so ISR can give logical pin to callback */
    pin_map[port][physical_pin] = (uint8_t)pin;

    return ARM_DRIVER_OK;
}

static int32_t ARM_GPIO_SetDirection(ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction) {
    if (pin >= (sizeof(pin_table)/sizeof(pin_table[0]))) return ARM_GPIO_ERROR_PIN;

    uint8_t physical_pin = pin_table[pin].pin;
    if (!PIN_IS_AVAILABLE(physical_pin)) return ARM_GPIO_ERROR_PIN;

    GPIO_Type* gpio = get_gpio_base(pin_table[pin].port);

    switch (direction) {
    case ARM_GPIO_OUTPUT:
        gpio->PDDR |= (1UL << physical_pin);
        break;
    case ARM_GPIO_INPUT:
        gpio->PDDR &= ~(1UL << physical_pin);
        break;
    default:
        return ARM_DRIVER_ERROR_PARAMETER;
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_GPIO_SetOutputMode(ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode) {
    /* For many MCUs output mode requires pin mux / configuration at PORT level.
       If needed, implement using DRIVER_PORT to modify PCR (open-drain etc).
       For now return OK (no-op) or error if pin invalid. */
    if (pin >= (sizeof(pin_table)/sizeof(pin_table[0]))) return ARM_GPIO_ERROR_PIN;
    (void)mode;
    return ARM_DRIVER_OK;
}

static int32_t ARM_GPIO_SetPullResistor(ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor) {
    if (pin >= (sizeof(pin_table)/sizeof(pin_table[0]))) return ARM_GPIO_ERROR_PIN;
    Driver_PortInstance port = pin_table[pin].port;
    uint8_t p = pin_table[pin].pin;
    if (!PIN_IS_AVAILABLE(p)) return ARM_GPIO_ERROR_PIN;

    if (resistor == ARM_GPIO_PULL_NONE) {
        DRIVER_PORT_PullConfig(port, p, DISABLE, 0);
    } else if (resistor == ARM_GPIO_PULL_UP) {
        DRIVER_PORT_PullConfig(port, p, ENABLE, 1);
    } else { /* PULL_DOWN */
        DRIVER_PORT_PullConfig(port, p, ENABLE, 0);
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_GPIO_SetEventTrigger(ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger) {
    if (pin >= (sizeof(pin_table)/sizeof(pin_table[0]))) return ARM_GPIO_ERROR_PIN;
    Driver_PortInstance port = pin_table[pin].port;
    uint8_t p = pin_table[pin].pin;
    if (!PIN_IS_AVAILABLE(p)) return ARM_GPIO_ERROR_PIN;

    Driver_PortIrqConfig irq = DRIVER_PORT_IRQ_DISABLED;
    switch (trigger) {
    case ARM_GPIO_TRIGGER_NONE:        irq = DRIVER_PORT_IRQ_DISABLED; break;
    case ARM_GPIO_TRIGGER_RISING_EDGE: irq = DRIVER_PORT_IRQ_RISING_EDGE; break;
    case ARM_GPIO_TRIGGER_FALLING_EDGE:irq = DRIVER_PORT_IRQ_FAILING_EDGE; break;
    case ARM_GPIO_TRIGGER_EITHER_EDGE: irq = DRIVER_PORT_IRQ_EITHER_EDGE; break;
    default: return ARM_DRIVER_ERROR_PARAMETER;
    }

    DRIVER_PORT_PinInterruptConfig(port, p, irq);
    return ARM_DRIVER_OK;
}

static void ARM_GPIO_SetOutput(ARM_GPIO_Pin_t pin, uint32_t val) {
    if (pin >= (sizeof(pin_table)/sizeof(pin_table[0]))) return;
    uint8_t p = pin_table[pin].pin;
    if (!PIN_IS_AVAILABLE(p)) return;
    GPIO_Type* gpio = get_gpio_base(pin_table[pin].port);
    if (val) {
        gpio->PSOR = (1UL << p); /* set */
    } else {
        gpio->PCOR = (1UL << p); /* clear */
    }
}

static uint32_t ARM_GPIO_GetInput(ARM_GPIO_Pin_t pin) {
    if (pin >= (sizeof(pin_table)/sizeof(pin_table[0]))) return ARM_GPIO_ERROR_PIN;
    uint8_t p = pin_table[pin].pin;
    if (!PIN_IS_AVAILABLE(p)) return ARM_GPIO_ERROR_PIN;
    GPIO_Type* gpio = get_gpio_base(pin_table[pin].port);
    return (uint32_t)((gpio->PDIR >> p) & 1UL);
}

/* Example application callback (moved here to be consistent) */
void Button_Event(ARM_GPIO_Pin_t pin, uint32_t event) {
    (void)event; /* we forward EITHER_EDGE; decide by reading input */

    /* on falling edge (button pressed when using pull-up), input becomes 0 */
    uint32_t level = ARM_GPIO_GetInput(pin);
    if (level == 0) {
        if (pin == BUTTON1) {
            static uint32_t red_state = 0;
            red_state ^= 1;
            ARM_GPIO_SetOutput(LED_RED, red_state);
        } else if (pin == BUTTON2) {
            static uint32_t green_state = 0;
            green_state ^= 1;
            ARM_GPIO_SetOutput(LED_GREEN, green_state);
        }
    }
}

/* Driver access structure (CMSIS-like) */
ARM_DRIVER_GPIO Driver_GPIO0 = {
    ARM_GPIO_Setup,
    ARM_GPIO_SetDirection,
    ARM_GPIO_SetOutputMode,
    ARM_GPIO_SetPullResistor,
    ARM_GPIO_SetEventTrigger,
    ARM_GPIO_SetOutput,
    ARM_GPIO_GetInput
};
