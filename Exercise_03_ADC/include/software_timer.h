#ifndef SOFTWARE_TIMER_H_
#define SOFTWARE_TIMER_H_

#include "stdint.h"
#include "my_nvic.h"
#include "S32K144.h"

#define MAX_SOFTWARE_TIMERS 10

void TIM_Init(void);
uint8_t TIM_IsFlag(uint8_t index);
uint8_t TIM_SetTime(uint8_t index, uint32_t duration_ms);

#endif /* SOFTWARE_TIMER_H_ */
