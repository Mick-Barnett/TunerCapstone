#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32l4xx_hal.h"
#include <stdint.h>

#define PRESCALE 0
#define SAMP_RATE 8000
#define PERIOD ((SystemCoreClock/SAMP_RATE)-1)

void TIM3_init(void);
void TIM3_IRQHandler(void);

#endif /* INC_TIMER_H_ */