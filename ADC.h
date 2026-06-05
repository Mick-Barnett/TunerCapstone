/**
 ******************************************************************************
 * @file    : ADC.h
 * @brief   : Header for ADC module
 * project  : EE 329 S'26 A5
 * authors  : Aaron Price Jr.
 * version  : 0.2
 * date     : 2026-06
 * target   : NUCLEO-L4A6ZG
 *
 * ADC1 channel 5, PA0 (Nucleo A0)
 * 12-bit resolution, single conversion, software trigger
 * VREF = 3.3V (internal)
 ******************************************************************************
 */
#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"
#include "delay.h"
#include <stdint.h>

/* ADC sample time selection */
#define SAMPLE_TIME 1

/* DSP constants */
#define ADC_BUFF_SIZE 4096
#define ADC_WND_SIZE 2048
#define ADC_HALF_WND 1024

/* Global ADC state */
extern volatile uint8_t ADC_wndw_ready;

/* Public functions */
void ADC_init(void);

void ADC_GetWindow(uint16_t window[ADC_WND_SIZE]);

void ADC1_2_IRQHandler(void);

#endif /* INC_ADC_H_ */
