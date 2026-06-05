/*
 *******************************************************************************
 * CAPSTONE: TIMER.c
 *******************************************************************************
 * @file           : TIMER.c
 * @brief          : Source file for TIM3 configuration and ADC sampling trigger
 * project         : EE 329 S'26 Capstone
 * authors         : Tyler Ragasa
 * version         : 0.1
 * date            : 06/2026
 * compiler        : STM32CubeIDE
 * target          : NUCLEO-L4A6ZG
 * clocks          : MSI system clock, TIM3 ARR function of SystemCoreClock
 * @attention      : TIM3_init must be called as last init/config
 ******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

#include "timer.h"
/*
 * Configures and enables TIM3 for ADC sampling clock freq 8 kHz
 * - Adapted from EE329 Lab Manual - A4
 */
void TIM3_init(void)
{
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;       // enable clock for TIM3
   TIM3->CR1 &= ~TIM_CR1_CEN;                  // stop timer before config
   TIM3->CR1 &= ~TIM_CR1_DIR;                  // up counting mode - DIR = 0
   TIM3->PSC = TIM3_PRESCALE;                  // Prescaler = 0
   TIM3->ARR = TIM3_PERIOD;                    // ARR = T = counts @4MHz
   TIM3->CNT = 0;                              // Start counting from 0
   TIM3->SR &= ~(TIM_SR_UIF);                  // Clear pending flag
   TIM3->DIER |= TIM_DIER_UIE;                 // enable update interupt
   NVIC->ISER[0] |= (1 << (TIM3_IRQn & 0x1F)); // set NVIC interrupt: 0x1F
   __enable_irq();                             // global IRQ enable
   TIM3->CR1 |= TIM_CR1_CEN;                   // start TIM3 CR1
}

/*
 * TIM3 Interrupt service routine
 * - Runs when TIM3 counter reaches ARR and sets update interrupt flag
 * - Clears UIF then starts ADC conversion
 */
void TIM3_IRQHandler(void)
{
   if (TIM3->SR & TIM_SR_UIF)
   {
      TIM3->SR &= ~TIM_SR_UIF;    // Clear flag
      ADC1->CR |= ADC_CR_ADSTART; // Start ADC conversion
   }
}
