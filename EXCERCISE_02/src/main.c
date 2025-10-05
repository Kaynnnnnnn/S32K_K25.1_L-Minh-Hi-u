/**
 * @file    main.c
 * @brief   Blink 3 LEDs on S32K144 every 3 seconds using SysTick-based delay.
 * @details Dành cho project bare-metal không dùng SDK, dùng IP_xxx register.
 */

#include "S32K144.h"
#include <stdint.h>

/* ------------------------------------------------------------------------- */
/* SysTick register definitions (bare-metal, không dùng CMSIS)              */
/* ------------------------------------------------------------------------- */
#define SYST_CSR   (*(volatile uint32_t*)0xE000E010U)   /* Control and Status Register */
#define SYST_RVR   (*(volatile uint32_t*)0xE000E014U)   /* Reload Value Register */
#define SYST_CVR   (*(volatile uint32_t*)0xE000E018U)   /* Current Value Register */

#define SYST_CSR_CLKSOURCE   (1UL << 2)
#define SYST_CSR_TICKINT     (1UL << 1)
#define SYST_CSR_ENABLE      (1UL << 0)
#define SYST_MAX_RELOAD      (0xFFFFFFUL)              /* 24-bit counter */

/* ------------------------------------------------------------------------- */
/* Macros                                                                    */
/* ------------------------------------------------------------------------- */
#define LED0_PIN        (0U)
#define LED1_PIN        (15U)
#define LED2_PIN        (16U)

#define LED_PORT        (IP_PTD)
#define LED_PORT_CTRL   (IP_PORTD)

#define DELAY_TIME_MS   (3000U)
#define CORE_CLOCK_HZ   (48000000U)   /* Clock mặc định 48 MHz */


/* ------------------------------------------------------------------------- */
/* Global variables                                                          */
/* ------------------------------------------------------------------------- */
static volatile uint32_t g_msTickCounter = 0U;

/* ------------------------------------------------------------------------- */
/* Function Prototypes                                                       */
/* ------------------------------------------------------------------------- */
static void LED_Init(void);
static void LED_ToggleAll(void);
static void Delay_Init(uint32_t coreClockHz);
static void Delay_ms(uint32_t delayMs);

/* ------------------------------------------------------------------------- */
/* SysTick Interrupt Handler                                                 */
/* ------------------------------------------------------------------------- */
void SysTick_Handler(void)
{
    g_msTickCounter++;
}

/* ------------------------------------------------------------------------- */
/* Function Definitions                                                      */
/* ------------------------------------------------------------------------- */
static void LED_Init(void)
{
    /* Bật clock cho PORTD */
    IP_PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

    /* Cấu hình các chân LED là GPIO */
    LED_PORT_CTRL->PCR[LED0_PIN] = PORT_PCR_MUX(1);
    LED_PORT_CTRL->PCR[LED1_PIN] = PORT_PCR_MUX(1);
    LED_PORT_CTRL->PCR[LED2_PIN] = PORT_PCR_MUX(1);

    /* Cấu hình hướng xuất */
    LED_PORT->PDDR |= (1U << LED0_PIN) | (1U << LED1_PIN) | (1U << LED2_PIN);

    /* Tắt LED ban đầu */
    LED_PORT->PSOR |= (1U << LED0_PIN) | (1U << LED1_PIN) | (1U << LED2_PIN);
}

static void LED_ToggleAll(void)
{
    LED_PORT->PTOR |= (1U << LED0_PIN) | (1U << LED1_PIN) | (1U << LED2_PIN);
}


static inline uint32_t SysTick_Config(uint32_t ticks)
{
    if ((ticks - 1UL) > SYST_MAX_RELOAD)
    {
        return 1UL;   /* Giá trị quá lớn (SysTick chỉ 24 bit) */
    }

    SYST_RVR = ticks - 1UL;                                      /* Nạp giá trị đếm */
    SYST_CVR = 0UL;                                              /* Reset current value */
    SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;

    return 0UL;  /* OK */
}


static void Delay_Init(uint32_t coreClockHz)
{
    /* Cấu hình SysTick mỗi 1 ms */
    (void)SysTick_Config(coreClockHz / 1000U);
}

static void Delay_ms(uint32_t delayMs)
{
    uint32_t startTick = g_msTickCounter;
    while ((g_msTickCounter - startTick) < delayMs)
    {
        /* busy wait */
    }
}

/* ------------------------------------------------------------------------- */
/* Main                                                                      */
/* ------------------------------------------------------------------------- */
int main(void)
{
    LED_Init();
    Delay_Init(CORE_CLOCK_HZ);

    while (1)
    {
        /* RED */
        LED_PORT->PCOR = (1U << LED0_PIN);   /* bật RED */
        LED_PORT->PSOR = (1U << LED1_PIN) | (1U << LED2_PIN); /* tắt GREEN + BLUE */
        Delay_ms(DELAY_TIME_MS);

        /* GREEN */
        LED_PORT->PCOR = (1U << LED1_PIN);   /* bật GREEN */
        LED_PORT->PSOR = (1U << LED0_PIN) | (1U << LED2_PIN); /* tắt RED + BLUE */
        Delay_ms(DELAY_TIME_MS);

        /* BLUE */
        LED_PORT->PCOR = (1U << LED2_PIN);   /* bật BLUE */
        LED_PORT->PSOR = (1U << LED0_PIN) | (1U << LED1_PIN); /* tắt RED + GREEN */
        Delay_ms(DELAY_TIME_MS);
    }

    return 0;
}
