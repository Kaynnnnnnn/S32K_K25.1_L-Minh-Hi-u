#include "S32K144.h"
#include "adc.h"
#include "software_timer.h"
#include "led.h"

int main(void)
{
    uint16_t adc_val;
    uint8_t led_val;

    ADC_Init();
    TIM_Init();
    LED_Init();

    TIM_SetTime(0, 10);  // đọc ADC mỗi 10ms

    while(1)
    {
        if(TIM_IsFlag(0))
        {
            TIM_SetTime(0,10);  // reset
            adc_val = ADC_Read_Channel(12); // biến trở
            led_val = adc_val;  // 0-255
            LED_SetBrightness(led_val);
        }

        LED_Update();  // PWM LED
    }
}
