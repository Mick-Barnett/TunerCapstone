/*******************************************************************************
 * @file           : main.h
 * @brief          : header file for main.c code
 * project         : EE 329 S'26 A8
 * authors         : Mick Barnett, Tyler Ragasa
 * version         : 0.4
 * date            : 06/1/2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * PIN ASSIGNMENTS
 *PA0 --> Analog Input
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "delay.h"
#include "ADC.h"
#include "FFT.h"
#include "Processor.h"
#include "TIMER.h"
#include "lpuart.h"
#include "LCD.h"

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

