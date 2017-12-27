#pragma once

//functions specific to this particular MCU Family

#include "includes.h"

volatile uint32_t systemUsTicks;

//functions specific to this particular MCU Family

extern uint32_t InlineMillis(void);
extern void InlineUpdateMillisClock (void);
extern uint32_t InlineMicros(void);
extern void InlineDelayUs(uint32_t uSec);
extern void InlineDelayMs(uint32_t mSec);
extern void inline_digital_hi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
<<<<<<< HEAD
extern void InlineDigitalLo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
=======
extern void inline_digital_lo(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
>>>>>>> 0b7ca046eb733b9335e68be2e0bf28e8a6a7e9b3
extern int InlineIsPinStatusHi(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern void DeInitGpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern void init_gpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t on);