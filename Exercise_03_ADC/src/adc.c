#include "adc.h"

uint8_t ADC_Init(void)
{
    /* Enable clock cho ADC0 */
    IP_PCC->PCCn[PCC_ADC0_INDEX] = 0;
    IP_PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;

    /* Disable ADC trước khi cấu hình */
    IP_ADC0->SC1[0] = ADC_SC1_ADCH_MASK;

    /* Cấu hình 8-bit, clock divider = 2 */
    IP_ADC0->CFG1 = ADC_CFG1_ADICLK(0) | ADC_CFG1_MODE(0) | ADC_CFG1_ADIV(1);

    /* Software trigger */
    IP_ADC0->SC2 = ADC_SC2_ADTRG(0);

    return ADC_SUCCESS;
}

uint16_t ADC_Read_Channel(uint8_t channel)
{
    IP_ADC0->SC1[0] = ADC_SC1_ADCH(channel);
    while (!(IP_ADC0->SC1[0] & ADC_SC1_COCO_MASK)) {}
    return (uint16_t)IP_ADC0->R[0];
}
