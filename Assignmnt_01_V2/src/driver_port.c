#include "driver_port.h"
#include "S32K144.h"
#include "nvic_custom.h"
static inline uint32_t get_pcc_index(Driver_PortInstance port)
{
    switch (port)
    {
        case DRIVER_PORTA: return PCC_PORTA_INDEX;
        case DRIVER_PORTB: return PCC_PORTB_INDEX;
        case DRIVER_PORTC: return PCC_PORTC_INDEX;
        case DRIVER_PORTD: return PCC_PORTD_INDEX;
        case DRIVER_PORTE: return PCC_PORTE_INDEX;
        default:           return PCC_PORTA_INDEX; /* fallback */
    }
}

static inline PORT_Type* get_port_base(Driver_PortInstance port)
{
    switch (port)
    {
        case DRIVER_PORTA: return IP_PORTA;
        case DRIVER_PORTB: return IP_PORTB;
        case DRIVER_PORTC: return IP_PORTC;
        case DRIVER_PORTD: return IP_PORTD;
        case DRIVER_PORTE: return IP_PORTE;
        default:           return IP_PORTA; /* fallback */
    }
}

static inline IRQn_Type get_port_irqn(Driver_PortInstance port)
{
    switch (port)
    {
        case DRIVER_PORTA: return PORTA_IRQn;
        case DRIVER_PORTB: return PORTB_IRQn;
        case DRIVER_PORTC: return PORTC_IRQn;
        case DRIVER_PORTD: return PORTD_IRQn;
        case DRIVER_PORTE: return PORTE_IRQn;
        default:           return PORTA_IRQn; /* fallback */
    }
}

/* ===== Callback storage ===== */
static Driver_PortCallback callbacks[DRIVER_PORT_COUNT] = {0};

/* ===== API Implementation ===== */

/* Enable PCC clock for the selected port */
void DRIVER_PORT_EnableClock(Driver_PortInstance port)
{
    uint32_t idx = get_pcc_index(port);
    IP_PCC->PCCn[idx] &= ~PCC_PCCn_CGC_MASK;  /* ensure clock off first */
    IP_PCC->PCCn[idx] |= PCC_PCCn_CGC_MASK;   /* enable clock */
}

/* Configure pin mux */
void DRIVER_PORT_PinMux(Driver_PortInstance port, uint8_t pin, Driver_PortMux mux)
{
    PORT_Type* base = get_port_base(port);
    base->PCR[pin] = (base->PCR[pin] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(mux);
}

/* Configure pull-up / pull-down resistor */
void DRIVER_PORT_PullConfig(Driver_PortInstance port,
                            uint8_t pin,
                            uint8_t enable,
                            uint8_t pullup)
{
    PORT_Type* base = get_port_base(port);

    if (enable)
    {
        base->PCR[pin] |= PORT_PCR_PE_MASK;
        if (pullup)
        {
            base->PCR[pin] |= PORT_PCR_PS_MASK;
        }
        else
        {
            base->PCR[pin] &= ~PORT_PCR_PS_MASK;
        }
    }
    else
    {
        base->PCR[pin] &= ~PORT_PCR_PE_MASK;
    }
}

/* Configure pin interrupt mode and enable NVIC */
void DRIVER_PORT_PinInterruptConfig(Driver_PortInstance port,
                                    uint8_t pin,
                                    Driver_PortIrqConfig irq_mode)
{
    PORT_Type* base = get_port_base(port);

    /* clear previous IRQC then set new */
    base->PCR[pin] = (base->PCR[pin] & ~PORT_PCR_IRQC_MASK) | PORT_PCR_IRQC(irq_mode);

    IRQn_Type irq = get_port_irqn(port);
    NVIC_SetPriority(irq, 2);
    NVIC_EnableIRQ(irq);
}

/* Register user callback */
void DRIVER_PORT_RegisterCallback(Driver_PortInstance port, Driver_PortCallback cb)
{
    if (port < DRIVER_PORT_COUNT)
    {
        callbacks[port] = cb;
    }
}

/* Clear ISFR interrupt flag */
void DRIVER_PORT_ClearInterruptFlag(Driver_PortInstance port, uint8_t pin)
{
    PORT_Type* base = get_port_base(port);
    base->ISFR = (1UL << pin); /* write 1 to clear */
}

/* ===== ISR Handlers ===== */
static inline void handle_isr(Driver_PortInstance port, PORT_Type* base)
{
    uint32_t flags = base->ISFR;
    base->ISFR = flags; /* clear all active flags */

    if (callbacks[port])
    {
        for (uint8_t i = 0; i < 32; ++i)
        {
            if (flags & (1UL << i))
            {
                callbacks[port](port, i); /* FIX: truyền đủ 2 tham số */
            }
        }
    }
}


void PORTA_IRQHandler(void) { handle_isr(DRIVER_PORTA, IP_PORTA); }
void PORTB_IRQHandler(void) { handle_isr(DRIVER_PORTB, IP_PORTB); }
void PORTC_IRQHandler(void) { handle_isr(DRIVER_PORTC, IP_PORTC); }
void PORTD_IRQHandler(void) { handle_isr(DRIVER_PORTD, IP_PORTD); }
void PORTE_IRQHandler(void) { handle_isr(DRIVER_PORTE, IP_PORTE); }

/* END OF FILE */
