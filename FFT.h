/**
 ******************************************************************************
 * @file    : FFT.h
 * @brief   : Header for FFT module. Contains function prototypes for obtaining
 * 			  frequency, noise filtering, and choosing a magnitude for harmonics
 * project  : EE 329 S'26 A5
 * authors  : Aaron Price Jr.
 * version  : 0.2
 * date     : 2026-05-19
 * target   : NUCLEO-L4A6ZG
 ******************************************************************************
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#define f_sample 8000 //8kHz sample rate
#define FFT_size 2048 // 2048 samples


#include "stm32l4xx.h"
#include "ADC.h"
#include <stdint.h>

void FFT_Init(void);
void FFT_Get_Freq(uint16_t adc_buffer);
void FFT_Noise_Filter(void);
void FFT_Get_Last_Magnitude();

#endif /* INC_ADC_H_ */
