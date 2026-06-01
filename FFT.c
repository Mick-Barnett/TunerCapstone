/**
 ******************************************************************************
 * @file    : FFT.c
 * @brief   : Contains functionality for anything related to the Fast Fourier
 * 			  transform for the guitar. Initialization, Spectral Filtering, and
 * 			  choosing the correct harmonic are all accounted for.
 * project  : EE 329 S'26 A5
 * authors  : Aaron Price Jr.
 * version  : 0.2
 * date     : 2026-05-19
 * target   : NUCLEO-L4A6ZG
 ******************************************************************************
 */

#include "FFT.h"

static complex_t fft_buffer[FFT_SIZE];
static float magnitude_buffer[FFT_SIZE / 2];
static float last_magnitude = 0.0f;


/**
 * @brief Initialize FFT module state.
 */
void FFT_Init(void)
{
    last_magnitude = 0.0f;
}


/* Swaps the position of 2 entries in an array */
static void FFT_Swap(complex_t *a, complex_t *b)
{
    complex_t temp = *a;
    *a = *b;
    *b = temp;
}


/* Takes the index of all 2048 samples, and changes their order, by putting them
 * in bit reverse order:
 *
 * Ex: sample[1] = sample[0000000001]
 * 	 sample[1024] = sample[10000000000]
 * 	 sample[1] becomes sample [1024] and vise versa after bitreverse 	 */
static void FFT_BitReverse(complex_t *x, uint16_t n)
{
    uint16_t j = 0;

    for (uint16_t i = 1; i < n; i++)
    {
        uint16_t bit = n >> 1;

        while (j & bit)
        {
            j ^= bit;
            bit >>= 1;
        }

        j ^= bit;

        if (i < j)
        {
            FFT_Swap(&x[i], &x[j]);
        }
    }
}


/*
 * @brief Perform an in-place radix-2 Cooley-Tukey FFT.
 * DOES NOT RETURN ANY VALUE:
 * Turns the 2048 time domain samples from microphone input, into 2048 frequency
 * domain samples, with real and complex components.
 */
void FFT_Compute(complex_t *x, uint16_t n)
{
    FFT_BitReverse(x, n);

    for (uint16_t len = 2; len <= n; len <<= 1)
    {
   	  //twiddle factor calculation for each transformation set
        float angle = -2.0f * M_PI / (float)len;
        float w_len_real = cosf(angle);
        float w_len_imag = sinf(angle);

        for (uint16_t i = 0; i < n; i += len)
        {
            float w_real = 1.0f;
            float w_imag = 0.0f;

            for (uint16_t j = 0; j < len / 2; j++)
            {
                complex_t even = x[i + j];
                complex_t odd  = x[i + j + len / 2];

                /* Multiply odd value by twiddle factor W */
                float odd_twiddled_real =
                    odd.real * w_real - odd.imag * w_imag;

                float odd_twiddled_imag =
                    odd.real * w_imag + odd.imag * w_real;

                /* Butterfly operation */
                x[i + j].real = even.real + odd_twiddled_real;
                x[i + j].imag = even.imag + odd_twiddled_imag;

                x[i + j + len / 2].real = even.real - odd_twiddled_real;
                x[i + j + len / 2].imag = even.imag - odd_twiddled_imag;

                /* Update twiddle factor */
                float next_w_real =
                    w_real * w_len_real - w_imag * w_len_imag;

                float next_w_imag =
                    w_real * w_len_imag + w_imag * w_len_real;

                w_real = next_w_real;
                w_imag = next_w_imag;
            }
        }
    }
}


/**
 * @brief Convert FFT output into magnitudes.
 *
 * Magnitude = sqrt(real^2 + imag^2)
 */
static void FFT_Compute_Magnitudes(void)
{
    for (uint16_t i = 0; i < FFT_SIZE / 2; i++)
    {
        float real = fft_buffer[i].real;
        float imag = fft_buffer[i].imag;

        magnitude_buffer[i] = sqrtf(real * real + imag * imag);
    }
}


/* Removes any noise frequencies with magnitudes under the nosie threshold */
void FFT_Noise_Filter(float *magnitudes, uint16_t length)
{
    const float noise_threshold = 1000.0f;

    for (uint16_t i = 0; i < length; i++)
    {
        if (magnitudes[i] < noise_threshold)
        {
            magnitudes[i] = 0.0f;
        }
    }
}


/**
 * @brief Find dominant frequency from ADC sample buffer.
 *
 * Converts ADC samples to centered complex values, computes FFT,
 * finds the strongest frequency bin, and returns its frequency.
 */
float FFT_Get_Freq(uint16_t *sample_buffer, uint32_t length)
{
    if (sample_buffer == 0 || length < FFT_SIZE)
    {
        return 0.0f;
    }

    /* Convert unsigned 12-bit ADC samples into centered FFT input */
    for (uint16_t i = 0; i < FFT_SIZE; i++)
    {
        fft_buffer[i].real = (float)sample_buffer[i] - 2048.0f;
        fft_buffer[i].imag = 0.0f;
    }

    FFT_Compute(fft_buffer, FFT_SIZE);

    FFT_Compute_Magnitudes();

    FFT_Noise_Filter(magnitude_buffer, FFT_SIZE / 2);

    uint16_t min_bin = (uint16_t)(70.0f / FFT_BIN_WIDTH_HZ);
    uint16_t max_bin = (uint16_t)(400.0f / FFT_BIN_WIDTH_HZ);

    uint16_t peak_bin = min_bin;
    float peak_magnitude = 0.0f;

    for (uint16_t i = min_bin; i <= max_bin; i++)
    {
        if (magnitude_buffer[i] > peak_magnitude)
        {
            peak_magnitude = magnitude_buffer[i];
            peak_bin = i;
        }
    }

    last_magnitude = peak_magnitude;

    return (float)peak_bin * FFT_BIN_WIDTH_HZ;
}


/**
 * @brief Return strongest magnitude from the last FFT.
 */
float FFT_Get_Last_Magnitude(void)
{
    return last_magnitude;
}

