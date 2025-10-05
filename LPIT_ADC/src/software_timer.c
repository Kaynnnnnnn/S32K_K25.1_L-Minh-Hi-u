/**
 * @file    software_timer.c
 * @brief   Implementation file for software timer module.
 * @details This file provides functions to initialize LPIT hardware timer
 *          and manage multiple software timers on top of it.
 *
 * @author  ADMIN
 * @date    Sep 30, 2025
 */

#include "software_timer.h"

/*==================================================================================================
 *                                      DEFINES
 *==================================================================================================*/

/**
 * @brief LPIT channel index used for software timer base tick.
 */
#define SOFTWARE_TIMER_LPIT_CHANNEL      (0U)

/**
 * @brief LPIT input clock frequency [Hz].
 */
#define LPIT_INPUT_CLOCK_HZ             (20000000UL)

/**
 * @brief Software timer tick period [ms].
 */
#define SOFTWARE_TIMER_TICK_MS          (1U)

/**
 * @brief LPIT timer load value.
 * @details LPIT runs with SPLLDIV2_CLK = 20 MHz. To get 1 ms period:
 *          TVAL = 20,000 - 1.
 */
#define LPIT_TIMER_LOAD_VALUE           ((LPIT_INPUT_CLOCK_HZ / 1000UL * SOFTWARE_TIMER_TICK_MS) - 1UL)


/*==================================================================================================
 *                                      PRIVATE VARIABLES
 *==================================================================================================*/

/**
 * @brief Countdown values for each software timer.
 * @note  Declared volatile because updated in ISR and accessed in main loop.
 */
static volatile uint32_t s_timerCounter[MAX_SOFTWARE_TIMERS];

/**
 * @brief Expiry flags for each software timer.
 * @note  Declared volatile for the same reason as s_timerCounter.
 */
static volatile uint8_t s_timerFlag[MAX_SOFTWARE_TIMERS];


/*==================================================================================================
 *                                      PRIVATE FUNCTIONS
 *==================================================================================================*/

/**
 * @brief Core update function for all software timers.
 * @details Decrements each active timer by 1 tick. When a timer reaches 0,
 *          the corresponding flag is set.
 * @note    This function should only be called from the LPIT ISR.
 */
static void SoftwareTimer_Run(void)
{
    for (uint8_t i = 0U; i < MAX_SOFTWARE_TIMERS; i++)
    {
        if (s_timerCounter[i] > 0U)
        {
            s_timerCounter[i]--;
            if (s_timerCounter[i] == 0U)
            {
                s_timerFlag[i] = 1U;
            }
        }
    }
}

/**
 * @brief Clear LPIT interrupt flag for the specified channel.
 * @param[in] channel LPIT channel index (0-3).
 */
static inline void SoftwareTimer_ClearInterruptFlag(uint8_t channel)
{
    /* Each TIFx bit is at offset 0~3, so we can clear directly using bit mask */
    IP_LPIT0->MSR = (1UL << channel);
}


/*==================================================================================================
 *                                      PUBLIC FUNCTIONS
 *==================================================================================================*/

void TIM_Init(void)
{
    /* 1. Enable clock for LPIT */
    IP_PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC_MASK;
    IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_PCS(6U);     /* SPLLDIV2_CLK */
    IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;

    /* 2. Software reset */
    IP_LPIT0->MCR |= LPIT_MCR_SW_RST_MASK;
    IP_LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;

    /* 3. Enable LPIT module */
    IP_LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;

    /* 4. Configure channel for periodic 1 ms tick */
    IP_LPIT0->TMR[SOFTWARE_TIMER_LPIT_CHANNEL].TCTRL = 0U;
    IP_LPIT0->TMR[SOFTWARE_TIMER_LPIT_CHANNEL].TCTRL |= LPIT_TMR_TCTRL_MODE(0U);
    IP_LPIT0->TMR[SOFTWARE_TIMER_LPIT_CHANNEL].TVAL  = LPIT_TIMER_LOAD_VALUE;
    IP_LPIT0->MIER |= LPIT_MIER_TIE0_MASK;

    /* 5. Enable operation in debug & doze modes */
    IP_LPIT0->MCR |= LPIT_MCR_DBG_EN_MASK | LPIT_MCR_DOZE_EN_MASK;

    /* 6. Enable interrupt in NVIC and start timer */
    NVIC->ISER[LPIT0_Ch0_IRQn / 32U] = (1UL << (LPIT0_Ch0_IRQn % 32U));
    IP_LPIT0->TMR[SOFTWARE_TIMER_LPIT_CHANNEL].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;

    /* 7. Initialize software timer arrays */
    for (uint8_t i = 0U; i < MAX_SOFTWARE_TIMERS; i++)
    {
        s_timerCounter[i] = 0U;
        s_timerFlag[i]    = 0U;
    }
}

uint8_t TIM_SetTime(uint8_t index, uint32_t duration_ms)
{
    if (index >= MAX_SOFTWARE_TIMERS)
    {
        return 1U; /* Error: invalid index */
    }

    s_timerCounter[index] = duration_ms;
    s_timerFlag[index]    = 0U;

    return 0U; /* Success */
}

uint8_t TIM_IsFlag(uint8_t index)
{
    if (index >= MAX_SOFTWARE_TIMERS)
    {
        return 0U; /* Invalid index: treat as not expired */
    }

    uint8_t flagStatus = s_timerFlag[index];
    s_timerFlag[index] = 0U;

    return flagStatus;
}

void LPIT0_Ch0_IRQHandler(void)
{
    SoftwareTimer_ClearInterruptFlag(SOFTWARE_TIMER_LPIT_CHANNEL);
    SoftwareTimer_Run();
}
