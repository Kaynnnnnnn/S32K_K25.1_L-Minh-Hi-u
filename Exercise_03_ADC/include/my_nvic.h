#ifndef MY_NVIC_H_
#define MY_NVIC_H_

#include <stdint.h>

#define     __I     volatile const
#define     __O     volatile
#define     __IO    volatile

typedef struct
{
  __IO uint32_t ISER[8U];
       uint32_t RESERVED0[24U];
  __IO uint32_t ICER[8U];
       uint32_t RESERVED1[24U];
  __IO uint32_t ISPR[8U];
       uint32_t RESERVED2[24U];
  __IO uint32_t ICPR[8U];
       uint32_t RESERVED3[24U];
  __I  uint32_t IABR[8U];
       uint32_t RESERVED4[56U];
  __IO uint8_t  IP[240U];
       uint32_t RESERVED5[644U];
  __O  uint32_t STIR;
}  NVIC_Type;

#define NVIC_BASE           (0xE000E100UL)
#define NVIC                ((NVIC_Type *) NVIC_BASE)

#endif /* MY_NVIC_H_ */
