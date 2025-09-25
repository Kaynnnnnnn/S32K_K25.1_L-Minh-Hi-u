#include "driver_GPIO.h"

// ✅ Chỉ khai báo, không định nghĩa lại
extern const ARM_DRIVER_GPIO Driver_GPIO;

#define LED_RED_PIN     15
#define LED_GREEN_PIN   16
#define BTN1_PIN        12
#define BTN2_PIN        13

static uint32_t led_red_state = 0;
static uint32_t led_green_state = 0;

// ✅ Không định nghĩa lại ARM_GPIO_SignalEvent
// Sử dụng callback đã đăng ký qua Setup()

void GPIO_EventCallback(ARM_GPIO_Pin_t pin, uint32_t event) {
    if (event & ARM_GPIO_EVENT_RISING_EDGE) {
        if (pin == BTN1_PIN) {
            led_red_state ^= 1;
            Driver_GPIO.SetOutput(LED_RED_PIN, led_red_state);
        } else if (pin == BTN2_PIN) {
            led_green_state ^= 1;
            Driver_GPIO.SetOutput(LED_GREEN_PIN, led_green_state);
        }
    }
}

int main(void) {
    Driver_GPIO.Setup(LED_RED_PIN, NULL);
    Driver_GPIO.SetDirection(LED_RED_PIN, ARM_GPIO_OUTPUT);
    Driver_GPIO.SetOutput(LED_RED_PIN, 0);

    Driver_GPIO.Setup(LED_GREEN_PIN, NULL);
    Driver_GPIO.SetDirection(LED_GREEN_PIN, ARM_GPIO_OUTPUT);
    Driver_GPIO.SetOutput(LED_GREEN_PIN, 0);

    Driver_GPIO.Setup(BTN1_PIN, GPIO_EventCallback);
    Driver_GPIO.SetDirection(BTN1_PIN, ARM_GPIO_INPUT);
    Driver_GPIO.SetPullResistor(BTN1_PIN, ARM_GPIO_PULL_UP);
    Driver_GPIO.SetEventTrigger(BTN1_PIN, ARM_GPIO_TRIGGER_RISING_EDGE);

    Driver_GPIO.Setup(BTN2_PIN, GPIO_EventCallback);
    Driver_GPIO.SetDirection(BTN2_PIN, ARM_GPIO_INPUT);
    Driver_GPIO.SetPullResistor(BTN2_PIN, ARM_GPIO_PULL_UP);
    Driver_GPIO.SetEventTrigger(BTN2_PIN, ARM_GPIO_TRIGGER_RISING_EDGE);

    while (1) {}
}
