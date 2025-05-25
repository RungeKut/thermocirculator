#ifndef __TIM_H
#define __TIM_H

#include "main.h"

void delay_ms(uint32_t count);
void TIM1_Init(void);
void TIM2_Init(void);
void HAL_InitTick(uint32_t TickFreq);
void HAL_Delay(uint32_t Delay);

#endif /* __TIM_H */
