#include "software_timer.h"

#define LPIT_CLK 20000  // tick 1ms @ SPLLDIV2_CLK 20MHz

volatile uint32_t timer_counter[MAX_SOFTWARE_TIMERS];
volatile uint8_t timer_flag[MAX_SOFTWARE_TIMERS];

/* Cập nhật timer */
static void TIM_TimerRun(void)
{
    for(uint8_t i=0; i<MAX_SOFTWARE_TIMERS; i++)
    {
        if(timer_counter[i] > 0) timer_counter[i]--;
        else timer_flag[i] = 1;
    }
}

/* Xóa flag ngắt LPIT channel */
static void TIM_ClearInterruptFlag(uint8_t ch)
{
    if(ch > 3) return;
    IP_LPIT0->MSR = 1 << ch;
}

void TIM_Init(void)
{
    /* Enable clock LPIT */
    IP_PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC_MASK;
    IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;

    /* Reset LPIT */
    IP_LPIT0->MCR |= LPIT_MCR_SW_RST_MASK;
    IP_LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;

    /* Enable module */
    IP_LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;

    /* Channel 0, tick 1ms */
    IP_LPIT0->TMR[0].TCTRL = 0;
    IP_LPIT0->TMR[0].TVAL = LPIT_CLK - 1;
    IP_LPIT0->MIER |= LPIT_MIER_TIE0_MASK;

    /* Debug / doze */
    IP_LPIT0->MCR |= LPIT_MCR_DBG_EN_MASK | LPIT_MCR_DOZE_EN_MASK;

    /* Enable NVIC */
    NVIC->ISER[LPIT0_Ch0_IRQn / 32] = (1 << (LPIT0_Ch0_IRQn % 32));

    /* Start timer */
    IP_LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;

    for(uint8_t i=0;i<MAX_SOFTWARE_TIMERS;i++)
    {
        timer_counter[i]=0;
        timer_flag[i]=0;
    }
}

uint8_t TIM_IsFlag(uint8_t idx)
{
    if(idx >= MAX_SOFTWARE_TIMERS) return 0xFF;
    uint8_t f = timer_flag[idx];
    timer_flag[idx]=0;
    return f;
}

uint8_t TIM_SetTime(uint8_t idx, uint32_t t_ms)
{
    if(idx >= MAX_SOFTWARE_TIMERS) return 0;
    timer_counter[idx] = t_ms;
    timer_flag[idx] = 0;
    return 1;
}

/* LPIT0 IRQ Handler */
void LPIT0_Ch0_IRQHandler(void)
{
    TIM_ClearInterruptFlag(0);
    TIM_TimerRun();
}
