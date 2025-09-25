#include "driver_GPIO.h"

#define GPIO_MAX_PIN 32

static uint32_t gpio_output_state[GPIO_MAX_PIN] = {0};
static ARM_GPIO_SignalEvent_t gpio_callbacks[GPIO_MAX_PIN] = {0};

static int32_t GPIO_Setup(ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event) {
    if (pin >= GPIO_MAX_PIN) return ARM_GPIO_ERROR_PIN;
    gpio_callbacks[pin] = cb_event;
    return ARM_DRIVER_OK;
}

static int32_t GPIO_SetDirection(ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction) {
    return ARM_DRIVER_OK;
}

static int32_t GPIO_SetOutputMode(ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode) {
    return ARM_DRIVER_OK;
}

static int32_t GPIO_SetPullResistor(ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor) {
    return ARM_DRIVER_OK;
}

static int32_t GPIO_SetEventTrigger(ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger) {
    return ARM_DRIVER_OK;
}

static void GPIO_SetOutput(ARM_GPIO_Pin_t pin, uint32_t val) {
    if (pin < GPIO_MAX_PIN) {
        gpio_output_state[pin] = val;
    }
}

static uint32_t GPIO_GetInput(ARM_GPIO_Pin_t pin) {
    if (pin < GPIO_MAX_PIN) {
        return gpio_output_state[pin];
    }
    return 0;
}

// ✅ Chỉ định nghĩa ARM_GPIO_SignalEvent ở đây
void ARM_GPIO_SignalEvent(ARM_GPIO_Pin_t pin, uint32_t event) {
    if (pin < GPIO_MAX_PIN && gpio_callbacks[pin]) {
        gpio_callbacks[pin](pin, event);
    }
}

// ✅ Chỉ định nghĩa Driver_GPIO ở đây
const ARM_DRIVER_GPIO Driver_GPIO = {
    GPIO_Setup,
    GPIO_SetDirection,
    GPIO_SetOutputMode,
    GPIO_SetPullResistor,
    GPIO_SetEventTrigger,
    GPIO_SetOutput,
    GPIO_GetInput
};
