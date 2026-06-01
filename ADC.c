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

// Initialize ADC parameters
volatile uint16_t ADC_buffer[ADC_BUFF_SIZE]={0};
volatile uint16_t ADC_buff_idx = 0;
volatile uint16_t ADC_samp_cnt = 0;
volatile uint16_t ADC_ref_idx = 0;
volatile uint8_t ADC_wndw_ready = 0;

void ADC_init(void) {
//run the ADC with a clock of at least 24Mhz
//single channel,single conversion software tirggered by ADC_CR_ADSTART bit
//12-bit resolution, 3.3V ful scale range, 6.5 clock sample time
//use GPIO pin configured for analog input, interrupt on end-of-conversion

	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;         // turn on clock for ADC
	// power up & calibrate ADC
	ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // clock source = HCLK/1
	ADC1->CR &= ~(ADC_CR_DEEPPWD);             // disable deep-power-down
	ADC1->CR |= (ADC_CR_ADVREGEN);          // enable V regulator - see RM 18.4.6
	delay_us(20);                              // wait 20us for ADC to power up
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);    // PA0=ADC1_IN5, single-ended
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF); // disable ADC, single-end calib
	ADC1->CR |= ADC_CR_ADCAL;                  // start calibration
	while (ADC1->CR & ADC_CR_ADCAL) {
		;
	}        // wait for calib to finish
	// enable ADC
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	ADC1->CR |= ADC_CR_ADEN;                   // enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY)) {
		;
	}    // wait for ADC Ready flag
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	// configure ADC sampling & sequencing
	ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);    // sequence = 1 conv., ch 5
	ADC1->SMPR1 |= (SAMPLE_TIME << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time
	ADC1->CFGR &= ~( ADC_CFGR_CONT |         // single conversion mode
			ADC_CFGR_EXTEN |         // h/w trig disabled for s/w trig
			ADC_CFGR_RES);        // 12-bit resolution
	// configure & enable ADC interrupt
	ADC1->IER |= ADC_IER_EOCIE;                // enable end-of-conv interrupt
	ADC1->ISR |= ADC_ISR_EOC;                  // set to clear EOC flag
	NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F)); // enable ADC interrupt service
	__enable_irq();                            // enable global interrupts
	// configure GPIO pin PA0
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);    // connect clock to GPIOA
	GPIOA->MODER |= (GPIO_MODER_MODE0);   // analog mode for PA0 (set MODER last)

	ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion
}

void ADC1_2_IRQHandler(void) {
	if (ADC_ISR_EOC & ADC1->ISR) {
		ADC_buffer[ADC_buff_idx] = ADC1->DR; // collect sample
		ADC_buff_idx++;
		// buffer full, reset index
		if (ADC_buff_idx >= ADC_BUFF_SIZE) {ADC_buff_idx = 0;}
		ADC_samp_cnt++;
		// collected next 1024 samples
		if (ADC_samp_cnt >= ADC_HALF_WND) {
			ADC_samp_cnt = 0;
			ADC_ref_idx = ADC_buff_idx; // marks end of sample window
			ADC_wndw_ready = 1; // global window ready flag
		}
		ADC1->ISR = ADC_ISR_EOC; // clear flag
	}
}

// ChatGPT Model 5.5 was used to assist with arithmetic / logic
void ADC_GetWindow(uint16_t window[SIZE]) {
	// determine starting index for sample window accomodating for array wrap
	end_idx = ADC_ref_idx
	start_idx = (end_idx + ADC_BUFF_SIZE - ADC_WND_SIZE) % ADC_BUFF_SIZE;
	for (uint16_t idx = 0; idx < ADC_WND_SIZE; idx++){
		window[idx] = ADC_buffer[(start_idx + idx) % ADC_BUFF_SIZE];
	}
	ADC_wndw_ready = 0;
}
