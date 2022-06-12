#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f4xx.h"

extern volatile uint16_t ADC_bat;
extern volatile uint16_t ADC_data[8];

void ADC_Init(void);
void ADC_InitS(void);
void ADC_ReadSensors(void);
void ACD_ReadBattery(void);

#endif /*__ADC_H__*/
