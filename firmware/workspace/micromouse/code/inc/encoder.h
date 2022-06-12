#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "stm32f4xx.h"

#define ENC_ML_ROTATIONS (((float)((int32_t)TIM2->CNT))/(6.0f * 51.45f))
#define ENC_MR_ROTATIONS (((float)((int32_t)TIM5->CNT))/(6.0f * 51.45f))
	
#define ENC_ML_VAL (TIM2->CNT)
#define ENC_MR_VAL (TIM5->CNT)

#define ENC_ML_RST() (TIM2->CNT = 0)
#define ENC_MR_RST() (TIM5->CNT = 0)
#define ENC_RST() TIM2->CNT = 0;TIM5->CNT = 0;TIM2->EGR |= TIM_EGR_UG;TIM5->EGR |= TIM_EGR_UG

void Encoder_Init(void);

#endif /*__ENCODER_H__*/
