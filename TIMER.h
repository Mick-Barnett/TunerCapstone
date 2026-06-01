#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32l4xx_hal.h"
#include <stdint.h>

#define PRESCALE 0
// MSI: 4 MHz, f_clk = 8 kHz
// T_MSI = 0.25 us, T_clk = 125 us
// T_clk = 500 * T_MSI
#define PERIOD 499

void TIM3_init(void);
void TIM3_IRQHandler(void);

#endif /* INC_TIMER_H_ */