/*
 *******************************************************************************
 * CAPSTONE: TIMER.h
 *******************************************************************************
 * @file           : TIMER.h
 * @brief          : Header for TIM3 module and ADC sampling timer config
 * project         : EE 329 S'26 Capstone
 * authors         : Tyler Ragasa
 * version         : 0.1
 * date            : 06/2026
 * compiler        : STM32CubeIDE
 * target          : NUCLEO-L4A6ZG
 * clocks          : TIM3 ARR derived from SystemCoreClock
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */
#ifndef INC_TIMER_H_
#define INC_TIMER_H_
/*----- Includes -----*/
#include "stm32l4xx_hal.h"
#include <stdint.h>
/*----- Defines -----*/
#define TIM3_PRESCALE 0
#define TIM3_SAMP_RATE 8000
#define TIM3_PERIOD ((SystemCoreClock / TIM3_SAMP_RATE) - 1)
/*----- Public Func. Prototypes -----*/
void TIM3_init(void);

#endif /* INC_TIMER_H_ */