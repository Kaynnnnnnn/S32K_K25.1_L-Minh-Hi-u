#include "led.h"

/* PWM duty cycle 0-255 */
static volatile uint8_t pwm_duty = 0;

void LED_Init(void)
{
    /* 1. Bật clock cho PORTB */
    IP_PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;

    /* 2. Chọn GPIO mode cho PTB0 */
    LED_PORT->PCR[LED_PIN] &= ~PORT_PCR_MUX_MASK;
    LED_PORT->PCR[LED_PIN] |= PORT_PCR_MUX(1); // GPIO

    /* 3. Cấu hình PTB0 là output */
    LED_GPIO->PDDR |= (1 << LED_PIN);

    /* 4. Tắt LED lúc khởi tạo */
    LED_GPIO->PSOR = (1 << LED_PIN);
}

void LED_SetBrightness(uint8_t value)
{
    pwm_duty = value;
}

/* PWM đơn giản, gọi trong loop chính */
void LED_Update(void)
{
    static uint8_t counter = 0;
    counter++;
    if(counter > 255) counter = 0;

    if(counter < pwm_duty) LED_GPIO->PCOR = (1 << LED_PIN); // LED ON
    else LED_GPIO->PSOR = (1 << LED_PIN);                  // LED OFF
}
