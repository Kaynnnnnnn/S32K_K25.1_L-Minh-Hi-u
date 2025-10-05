#ifndef LED_H_
#define LED_H_

#include "S32K144.h"
#include <stdint.h>

/* LED nối vào PTB0 */
#define LED_PORT   IP_PORTB
#define LED_GPIO   IP_PTB
#define LED_PIN    0

/* Hàm điều khiển LED */
void LED_Init(void);
void LED_SetBrightness(uint8_t value);
void LED_Update(void);

#endif /* LED_H_ */
