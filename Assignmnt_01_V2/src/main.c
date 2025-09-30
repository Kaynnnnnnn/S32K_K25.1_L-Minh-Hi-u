#include "driver_gpio.h"
#include "S32K144.h"

/* External reference to GPIO driver */
extern ARM_DRIVER_GPIO Driver_GPIO0;

/* Main application entry point */
int main(void)
{
    /* Configure Red LED pin as output */
    Driver_GPIO0.Setup(LED_RED, NULL);
    Driver_GPIO0.SetDirection(LED_RED, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetOutput(LED_RED, 0);   /* Start with LED OFF */

    /* Configure Green LED pin as output */
    Driver_GPIO0.Setup(LED_GREEN, NULL);
    Driver_GPIO0.SetDirection(LED_GREEN, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetOutput(LED_GREEN, 0); /* Start with LED OFF */

    /* Configure Button 1 as input with pull-up resistor and falling edge trigger */
    Driver_GPIO0.Setup(BUTTON1, Button_Event);
    Driver_GPIO0.SetDirection(BUTTON1, ARM_GPIO_INPUT);
    Driver_GPIO0.SetPullResistor(BUTTON1, ARM_GPIO_PULL_UP);
    Driver_GPIO0.SetEventTrigger(BUTTON1, ARM_GPIO_TRIGGER_FALLING_EDGE);

    /* Configure Button 2 as input with pull-up resistor and falling edge trigger */
    Driver_GPIO0.Setup(BUTTON2, Button_Event);
    Driver_GPIO0.SetDirection(BUTTON2, ARM_GPIO_INPUT);
    Driver_GPIO0.SetPullResistor(BUTTON2, ARM_GPIO_PULL_UP);
    Driver_GPIO0.SetEventTrigger(BUTTON2, ARM_GPIO_TRIGGER_FALLING_EDGE);

    /* Infinite loop: MCU waits for interrupts */
    while (1)
    {
        __asm("WFI");   /* Wait For Interrupt - low power standby */
    }

    /* Code never reaches here */
    return 0;
}
