#include "timer.h"

// Adapted from EE329 Lab Manual - A4 
void TIM3_init(void) {
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;           // enable clock for TIM3
   TIM3->CR1 &= ~TIM_CR1_CEN;								// stop timer before config
   TIM3->CR1 &= ~TIM_CR1_DIR;                      // up counting mode - DIR = 0
   TIM3->PSC = PRESCALE;                     		// Prescaler = 0
   TIM3->ARR = PERIOD;                             // ARR = T = counts @4MHz
   TIM3->CNT = 0;                                  // Start counting from 0

   TIM3->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // Clear pending flags
   TIM3->DIER |= TIM_DIER_CC1IE; 					   // enable CC1 only

   NVIC->ISER[0] |= (1 << (TIM3_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
   __enable_irq();                                 // global IRQ enable
   TIM3->CR1 |= TIM_CR1_CEN;                       // start TIM3 CR1
}


void TIM3_IRQHandler(void)
{
   if (TIM3->SR & TIM_SR_UIF) {
      TIM3->SR &= ~TIM_SR_UIF; // Clear flag
      ADC1->CR |= ADC_CR_ADSTART;
   }
}
