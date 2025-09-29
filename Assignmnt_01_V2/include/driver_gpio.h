#ifndef DRIVER_GPIO_H_
#define DRIVER_GPIO_H_

#include "driver_common.h"
#include "driver_port.h"

#define ENABLE   1
#define DISABLE  0

#define BUTTON1    0
#define BUTTON2    1
#define LED_RED    2
#define LED_GREEN  3

typedef uint32_t ARM_GPIO_Pin_t;

typedef struct {
	Driver_PortInstance port;
    uint8_t             pin;
} pin_desp_t;

extern const pin_desp_t pin_table[];

/* Per-port each has up to 32 pins */
#define PINS_PER_PORT      (32U)
#define PIN_IS_AVAILABLE(pin)   ((pin) < PINS_PER_PORT)

typedef enum {
  ARM_GPIO_INPUT,
  ARM_GPIO_OUTPUT
} ARM_GPIO_DIRECTION;

typedef enum {
  ARM_GPIO_PUSH_PULL,
  ARM_GPIO_OPEN_DRAIN
} ARM_GPIO_OUTPUT_MODE;

typedef enum {
  ARM_GPIO_PULL_NONE,
  ARM_GPIO_PULL_UP,
  ARM_GPIO_PULL_DOWN
} ARM_GPIO_PULL_RESISTOR;

typedef enum {
  ARM_GPIO_TRIGGER_NONE,
  ARM_GPIO_TRIGGER_RISING_EDGE,
  ARM_GPIO_TRIGGER_FALLING_EDGE,
  ARM_GPIO_TRIGGER_EITHER_EDGE
} ARM_GPIO_EVENT_TRIGGER;

#define ARM_GPIO_ERROR_PIN  (ARM_DRIVER_ERROR_SPECIFIC - 1)
/* GPIO event masks (for ARM_GPIO_SignalEvent callback) */
#define ARM_GPIO_EVENT_RISING_EDGE   (1UL << 0)  /* Rising-edge detected  */
#define ARM_GPIO_EVENT_FALLING_EDGE  (1UL << 1)  /* Falling-edge detected */
#define ARM_GPIO_EVENT_EITHER_EDGE   (1UL << 2)  /* Either edge detected   */

typedef void (*ARM_GPIO_SignalEvent_t) (ARM_GPIO_Pin_t pin, uint32_t event);
void Button_Event (ARM_GPIO_Pin_t pin, uint32_t event);

typedef struct _ARM_DRIVER_GPIO {
  int32_t  (*Setup)(ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event);
  int32_t  (*SetDirection)(ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction);
  int32_t  (*SetOutputMode)(ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode);
  int32_t  (*SetPullResistor)(ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor);
  int32_t  (*SetEventTrigger)(ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger);
  void     (*SetOutput)(ARM_GPIO_Pin_t pin, uint32_t val);
  uint32_t (*GetInput)(ARM_GPIO_Pin_t pin);
} const ARM_DRIVER_GPIO;
/* Export driver instance (đảm bảo định nghĩa ở driver_gpio.c là `const`) */
extern const ARM_DRIVER_GPIO Driver_GPIO0;

#endif /* DRIVER_GPIO_H_ */
