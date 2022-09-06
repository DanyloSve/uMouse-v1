#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f4xx.h"

#define PWM_SetDutyTim4(duty) TIM4->CCR2 = (uint16_t) (duty); 
#define PWM_SetDutyTim3(duty) TIM3->CCR1 = (uint16_t) (duty);

void PWM_Init(void);

#endif /*__PWM_H__*/