/*
 *******************************************************************************
 * CAPSTONE: ADC
 *******************************************************************************
 * @file           : ADC.c
 * @brief          : Src file for ADC.c code
 * project         : EE 329 S'26 A8
 * authors         : Mick Barnett, Tyler Ragasa
 * version         : 0.4
 * date            : 06/1/2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 24 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * PIN ASSIGNMENTS
 *PA0 --> Analog Input
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */
#include "ADC.h"

volatile uint16_t ADC_buffer[ADC_BUFF_SIZE] = {0};
volatile uint16_t ADC_buff_idx = 0;
volatile uint16_t ADC_samp_cnt = 0;
volatile uint16_t ADC_ref_idx = 0;
volatile uint8_t ADC_wndw_ready = 0;

void ADC_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER |= GPIO_MODER_MODE0;

    ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos);

    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC1->CR |= ADC_CR_ADVREGEN;
    delay_us(20);

    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_5;

    ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL) { }

    ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) { }
    ADC1->ISR |= ADC_ISR_ADRDY;

    ADC1->SQR1 &= ~ADC_SQR1_SQ1;
    ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);

    ADC1->SMPR1 &= ~ADC_SMPR1_SMP5;
    ADC1->SMPR1 |= (SAMPLE_TIME << ADC_SMPR1_SMP5_Pos);

    ADC1->CFGR &= ~(ADC_CFGR_CONT | ADC_CFGR_EXTEN | ADC_CFGR_RES);

    ADC1->IER |= ADC_IER_EOCIE;
    ADC1->ISR |= ADC_ISR_EOC;

    NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F));
    __enable_irq();

    ADC1->CR |= ADC_CR_ADSTART;
}

void ADC1_2_IRQHandler(void)
{
    if (ADC1->ISR & ADC_ISR_EOC)
    {
        ADC_buffer[ADC_buff_idx] = ADC1->DR; //collect sample

        ADC_buff_idx++;								//buffer full, reset index
        if (ADC_buff_idx >= ADC_BUFF_SIZE)
        {
            ADC_buff_idx = 0;
        }

        ADC_samp_cnt++;								//collected next 1024 samples

        if (ADC_samp_cnt >= ADC_HALF_WND)
        {
            ADC_samp_cnt = 0;
            ADC_ref_idx = ADC_buff_idx;		//marks end of sample window
            ADC_wndw_ready = 1;					//global window ready flag
        }

        ADC1->ISR = ADC_ISR_EOC;					//clear flag

        ADC1->CR |= ADC_CR_ADSTART;				//start next conversion
    }
}

// ChatGPT Model 5.5 was used to assist with arithmetic / logic
void ADC_GetWindow(uint16_t window[ADC_WND_SIZE])
{
	// determine starting index for sample window accomodating for array wrap
    uint16_t end_idx = ADC_ref_idx;
    uint16_t start_idx =
        (end_idx + ADC_BUFF_SIZE - ADC_WND_SIZE) % ADC_BUFF_SIZE;

    for (uint16_t i = 0; i < ADC_WND_SIZE; i++)
    {
        window[i] = ADC_buffer[(start_idx + i) % ADC_BUFF_SIZE];
    }

    ADC_wndw_ready = 0;
}
