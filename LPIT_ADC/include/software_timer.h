/**
 * @file    software_timer.h
 * @brief   Software timer library based on a hardware timer tick.
 * @details This library allows multiple software timers to run on a single
 *          hardware timer (e.g., LPIT). It relies on a periodic tick
 *          interrupt to decrement counters and set flags when timers expire.
 *
 * @author  ADMIN
 * @date    Sep 30, 2025
 */

#ifndef SOFTWARE_TIMER_H_
#define SOFTWARE_TIMER_H_

/*==================================================================================================
 *                                        INCLUDES
 *==================================================================================================*/
#include "stdint.h"
#include "S32K144.h"
#include "my_nvic.h"

/*==================================================================================================
 *                                        DEFINES
 *==================================================================================================*/

/**
 * @brief Maximum number of software timers supported by the library.
 */
#define MAX_SOFTWARE_TIMERS     (10U)

/*==================================================================================================
 *                                    PUBLIC FUNCTION PROTOTYPES
 *==================================================================================================*/

/**
 * @brief   Initialize the hardware timer module.
 * @details Configures a hardware timer to generate a periodic interrupt (tick),
 *          typically every 1 ms. Must be called once before using any software timer.
 */
void TIM_Init(void);

/**
 * @brief   Check whether a specific software timer has expired.
 * @note    This function is intended to be called repeatedly in the main loop (polling).
 *          When it returns 1, the internal flag will be automatically cleared.
 *
 * @param[in] index  Index of the software timer to check (0 to MAX_SOFTWARE_TIMERS - 1)
 * @return uint8_t
 *         - 1: Timer has expired
 *         - 0: Timer is still running
 */
uint8_t TIM_IsFlag(uint8_t index);

/**
 * @brief   Set the duration and start a specific software timer.
 * @details Sets the countdown value for the specified timer. The timer starts
 *          immediately. Use TIM_IsFlag() to check for expiration.
 *
 * @param[in] index        Index of the software timer to set (0 to MAX_SOFTWARE_TIMERS - 1)
 * @param[in] duration_ms  Countdown duration in milliseconds (ms)
 * @return uint8_t
 *         - 1: Invalid index
 *         - 0: Timer set successfully
 */
uint8_t TIM_SetTime(uint8_t index, uint32_t duration_ms);

#endif /* SOFTWARE_TIMER_H_ */
