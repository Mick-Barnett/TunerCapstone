#include "timer.h"

/*----- Local Defines -----*/
#define TIM3_PRESCALE 0
#define TIM3_SAMP_RATE 8000
#define TIM3_PERIOD ((SystemCoreClock / TIM3_SAMP_RATE) - 1)
// Lightly adapted from EE329 Lab Manual - A4
void TIM3_init(void)
{
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;       // enable clock for TIM3
   TIM3->CR1 &= ~TIM_CR1_CEN;                  // stop timer before config
   TIM3->CR1 &= ~TIM_CR1_DIR;                  // up counting mode - DIR = 0
   TIM3->PSC = TIM3_PRESCALE;                  // Prescaler = 0
   TIM3->ARR = TIM3_PERIOD;                    // ARR = T = counts @4MHz
   TIM3->CNT = 0;                              // Start counting from 0
   TIM3->SR &= ~(TIM_SR_UIF);                  // Clear pending flag
   TIM3->DIER |= TIM_DIER_UIF;                 // enable update interupt
   NVIC->ISER[0] |= (1 << (TIM3_IRQn & 0x1F)); // set NVIC interrupt: 0x1F
   __enable_irq();                             // global IRQ enable
   TIM3->CR1 |= TIM_CR1_CEN;                   // start TIM3 CR1
}

void TIM3_IRQHandler(void)
{
   if (TIM3->SR & TIM_SR_UIF)
   {
      TIM3->SR &= ~TIM_SR_UIF;    // Clear flag
      ADC1->CR |= ADC_CR_ADSTART; // Start ADC conversion
   }
}
