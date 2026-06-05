/**
 ******************************************************************************
 * @file    : FFT.h
 * @brief   : Header for FFT module. Contains function prototypes for obtaining
 *            frequency, noise filtering, and choosing a magnitude peak for
 *            harmonics.
 * project  : EE 329 S'26 A5
 * authors  : Aaron Price Jr.
 * version  : 0.2
 * date     : 2026-05-19
 * target   : NUCLEO-L4A6ZG
 ******************************************************************************
 */

#ifndef INC_FFT_H_
#define INC_FFT_H_

#include <stdint.h>
#include <math.h>

/*
 * FFT size must match ADC window size
 */
#define FFT_SIZE 2048
#define ADC_SAMPLE_RATE_HZ 8000.0f

#define FFT_BIN_WIDTH_HZ \
    (ADC_SAMPLE_RATE_HZ / (float)FFT_SIZE)

/* Complex number structure */
typedef struct
{
    float real;
    float imag;
}
complex_t;

/*-------------------------FUNCTION PROTOTYPES------------------------------- */

void FFT_Init(void);
void FFT_Compute(complex_t *x, uint16_t n);
float FFT_Get_Freq(uint16_t *sample_buffer,
                   uint32_t length);
float FFT_Get_Last_Magnitude(void);
float FFT_Get_Last_Peak_Bin(void);
float FFT_Get_Magnitude_At_Bin(uint16_t bin);

#endif /* INC_FFT_H_ */
