#ifndef __MOTORS_H__
#define __MOTORS_H__

#include "stm32f4xx.h"

#define MOTORS_DRCTN_FRWRD  1
#define MOTORS_DRCTN_BCKWRD 0

#define PWM_SetDutyTim4(duty) TIM4->CCR2 = (uint16_t) (duty); 
#define PWM_SetDutyTim3(duty) TIM3->CCR1 = (uint16_t) (duty);

void Motors_Init(void);

void Motors_M1(const uint16_t speed, const uint8_t direction);

void Motors_M2(const uint16_t speed, const uint8_t direction);

#endif /*__MOTORS_H__*/