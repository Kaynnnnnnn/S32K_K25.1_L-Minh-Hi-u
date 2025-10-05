/**
 * @file      main.c
 * @author    Le Minh Hieu
 * @brief     Simple LED blinking example using software timer and LPIT on S32K144
 */

/*==================================================================================================
 *                                        INCLUDES
 *==================================================================================================*/
#include "S32K144.h"
#include "system_S32K144.h"
#include "software_timer.h"

/*==================================================================================================
 *                                        MACROS
 *==================================================================================================*/

/** GPIO direction */
#define GPIO_PIN_INPUT      (0U)
#define GPIO_PIN_OUTPUT     (1U)

/** Blue LED pin configuration (D0 - PTD0 on S32K144 EVB) */
#define BLUE_LED_PIN        (0U)
#define BLUE_LED_PORT       IP_PORTD
#define BLUE_LED_GPIO       IP_PTD

/* Software timer index for LED blinking */
#define LED_TIMER_INDEX     (0U)

/* Blink period in ms */
#define LED_BLINK_PERIOD_MS (1000U)

/*==================================================================================================
 *                                    FUNCTION PROTOTYPES
 *==================================================================================================*/
static void Clock_Init_System_SPLL(void);
static void GPIO_EnablePortClock(uint8_t port_index);
static void GPIO_InitPin(PORT_Type* port, GPIO_Type* gpio_port, uint8_t pin_number, uint8_t direction);
static inline void GPIO_TogglePin(GPIO_Type* gpio_port, uint8_t pin_number);
static inline void GPIO_ClearPin(GPIO_Type* gpio_port, uint8_t pin_number);

/*==================================================================================================
 *                                        MAIN FUNCTION
 *==================================================================================================*/
int main(void)
{
    /* 1. Initialize system clock to SPLL (80 MHz core, 40 MHz bus, 20 MHz slow) */
    Clock_Init_System_SPLL();
    SystemCoreClockUpdate();

    /* 2. Enable clock for PORTD (LED port) */
    GPIO_EnablePortClock(PCC_PORTD_INDEX);

    /* 3. Configure LED pin as output and set to LOW initially */
    GPIO_InitPin(BLUE_LED_PORT, BLUE_LED_GPIO, BLUE_LED_PIN, GPIO_PIN_OUTPUT);
    GPIO_ClearPin(BLUE_LED_GPIO, BLUE_LED_PIN);

    /* 4. Initialize software timer (LPIT + timer array) */
    TIM_Init();

    /* 5. Start software timer for LED blinking */
    TIM_SetTime(LED_TIMER_INDEX, LED_BLINK_PERIOD_MS);

    /* 6. Main loop */
    for (;;)
    {
        /* Poll timer flag */
        if (TIM_IsFlag(LED_TIMER_INDEX))
        {
            GPIO_TogglePin(BLUE_LED_GPIO, BLUE_LED_PIN);

            /* Restart the timer */
            TIM_SetTime(LED_TIMER_INDEX, LED_BLINK_PERIOD_MS);
        }
    }

    /* Code should never reach here */
    return 0;
}

/*==================================================================================================
 *                                        HELPER FUNCTIONS
 *==================================================================================================*/

static inline void GPIO_TogglePin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PTOR = (1UL << pin_number);
}

static inline void GPIO_ClearPin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PCOR = (1UL << pin_number);
}

static void GPIO_InitPin(PORT_Type* port, GPIO_Type* gpio_port, uint8_t pin_number, uint8_t direction)
{
    /* Set pin mux to GPIO (ALT1) */
    port->PCR[pin_number] = PORT_PCR_MUX(1U);

    /* Configure direction */
    if (direction == GPIO_PIN_OUTPUT)
    {
        gpio_port->PDDR |= (1UL << pin_number);
    }
    else
    {
        gpio_port->PDDR &= ~(1UL << pin_number);
    }
}

static void GPIO_EnablePortClock(uint8_t port_index)
{
    IP_PCC->PCCn[port_index] |= PCC_PCCn_CGC_MASK;
}

/*==================================================================================================
 *                                        SYSTEM CLOCK SETUP
 *==================================================================================================*/
static void Clock_Init_System_SPLL(void)
{
    /* 1. Configure SOSC (8 MHz external crystal) */
    IP_SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1U) | SCG_SOSCDIV_SOSCDIV2(1U);
    IP_SCG->SOSCCFG = SCG_SOSCCFG_RANGE(3U) | SCG_SOSCCFG_EREFS_MASK;
    IP_SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;
    while ((IP_SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) == 0U) { }

    /* 2. Configure SPLL (8 MHz * (MULT+16)/PREDIV) = 80 MHz */
    IP_SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
    IP_SCG->SPLLCFG  = SCG_SPLLCFG_PREDIV(0U) | SCG_SPLLCFG_MULT(4U); /* MULT=20 */
    IP_SCG->SPLLDIV  = SCG_SPLLDIV_SPLLDIV1(1U) | SCG_SPLLDIV_SPLLDIV2(3U);
    IP_SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
    while ((IP_SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) == 0U) { }

    /* 3. Switch system clock to SPLL */
    IP_SCG->RCCR = SCG_RCCR_SCS(6U)          /* SPLL as system clock */
                 | SCG_RCCR_DIVCORE(0U)      /* Core = 80 MHz */
                 | SCG_RCCR_DIVBUS(1U)       /* Bus = 40 MHz */
                 | SCG_RCCR_DIVSLOW(3U);     /* Slow = 20 MHz */

    while (((IP_SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) != 6U) { }
}
