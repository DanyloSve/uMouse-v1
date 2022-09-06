#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include "stm32f10x.h"


#include "spi1.h"
#include "timer_delays.h"

void RCC_Init(void);
void GPIO_Init(void);

#endif /*MAIN_H_*/
