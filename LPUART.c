#ifndef INC_LPUART_H_
#define INC_LPUART_H_

#include "stm32l4xx.h"
#include <stdint.h>

/* Initialization */
void LPUART_init(void);

/* Basic terminal functions */
void LPUART_print(const char *message);

char LPUART_read_char(void);

void LPUART_clear_screen(void);

void LPUART_cursor_home(void);

/* Numeric printing */
void LPUART_print_uint32(uint32_t value);

void LPUART_print_float_2(float value);

/* FFT debug display */
void LPUART_print_FFT_debug(float peak_bin,
                            float peak_freq,
                            float peak_mag);

void LPUART_print_FFT_two_peaks(uint16_t bin1,
                                float freq1,
                                float mag1,
                                uint16_t bin2,
                                float freq2,
                                float mag2);

#endif /* INC_LPUART_H_ */
