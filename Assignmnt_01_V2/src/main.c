#include "driver_gpio.h"
extern ARM_DRIVER_GPIO Driver_GPIO0;

int main(void) {
    /* LED Setup */
    Driver_GPIO0.Setup(LED_RED, NULL);
    Driver_GPIO0.SetDirection(LED_RED, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetOutput(LED_RED, 0);

    Driver_GPIO0.Setup(LED_GREEN, NULL);
    Driver_GPIO0.SetDirection(LED_GREEN, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetOutput(LED_GREEN, 0);

    /* Button Setup (use Button_Event callback) */
    Driver_GPIO0.Setup(BUTTON1, Button_Event);
    Driver_GPIO0.SetDirection(BUTTON1, ARM_GPIO_INPUT);
    Driver_GPIO0.SetPullResistor(BUTTON1, ARM_GPIO_PULL_UP);
    Driver_GPIO0.SetEventTrigger(BUTTON1, ARM_GPIO_TRIGGER_FALLING_EDGE);

    Driver_GPIO0.Setup(BUTTON2, Button_Event);
    Driver_GPIO0.SetDirection(BUTTON2, ARM_GPIO_INPUT);
    Driver_GPIO0.SetPullResistor(BUTTON2, ARM_GPIO_PULL_UP);
    Driver_GPIO0.SetEventTrigger(BUTTON2, ARM_GPIO_TRIGGER_FALLING_EDGE);

    while (1) {
        /* app idle - all handled in interrupts */
    }

    return 0;
}
