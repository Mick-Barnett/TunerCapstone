/**
 ******************************************************************************
 * @file    : FFT.h
 * @brief   : Header for FFT module. Contains function prototypes for obtaining
 * 			  frequency, noise filtering, and choosing a magnitude peak for
 * 			  harmonics.
 * project  : EE 329 S'26 A5
 * authors  : Aaron Price Jr.
 * version  : 0.2
 * date     : 2026-05-19
 * target   : NUCLEO-L4A6ZG
 ******************************************************************************
 */

#ifndef INC_FFT_H_
#define INC_FFT_H_

#include "stm32l4xx.h"
#include "ADC.h"
#include <stdint.h>
#include <math.h>

#define f_sample 8000 //8kHz sample rate
#define FFT_size 2048 // 2048 samples
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


/* Creates an object class for real and imaginary components for the FFT.
 * x = real + j(imag)
 */
typedef struct
{
	float real;
	float imag;
} complex_t;

//-------------------------FUNCTION PROTOTYPES----------------------------------
void FFT_Init(void);
void FFT_Get_Freq(uint16_t adc_buffer, uint32_t length);
void FFT_Noise_Filter(float magnitudes, uint32_t length);
void FFT_Get_Last_Magnitude();

#endif /* INC_FFT_H_ */
