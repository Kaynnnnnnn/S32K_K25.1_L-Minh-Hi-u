#ifndef ADC_H_
#define ADC_H_

#include "S32K144.h"
#include <stdint.h>

#define ADC_SUCCESS     0
#define ADC_INIT_FAIL   1

/* Khởi tạo ADC0 8-bit */
uint8_t ADC_Init(void);

/* Đọc giá trị ADC từ kênh (0-31) */
uint16_t ADC_Read_Channel(uint8_t channel);

#endif /* ADC_H_ */
