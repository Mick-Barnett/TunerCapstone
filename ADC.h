#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx_hal.h"
#include "delay.h"
#include <stdint.h>

// ch 5 Sample Time: 1 = 6.5 clocks, 4 = 47.5 clocks, 7 = 640.5 clocks
#define SAMPLE_TIME 1 
// DSP Constants
#define ADC_BUFF_SIZE 4096
#define ADC_WND_SIZE 2048
#define ADC_HALF_WND 1024

static volatile uint16_t ADC_buffer[ADC_BUFF_SIZE];
static volatile uint16_t ADC_buff_idx;
static volatile uint16_t ADC_samp_cnt;
static volatile uint16_t ADC_ref_idx;
extern volatile uint8_t ADC_wndw_ready;

void ADC_init(void);
void ADC1_2_IRQHandler(void);
void ADC_GetWindow(uint16_t window[SIZE]);

#endif
