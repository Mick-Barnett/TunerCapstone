/*
 * delay.c
 *
 *  Created on: Apr 26, 2026
 *      Author: mickp
 */


#include <delay.h>
// configure SysTick timer for use with delay_us().
// warning: breaks HAL_delay() by disabling interrupts for shorter delay timing.
void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |     	// enable SysTick Timer
                     SysTick_CTRL_CLKSOURCE_Msk); 	// select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  	// disable interrupt
}
void delay_us(const uint32_t time_us) {
	// set the counts for the specified delay
	SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
	SysTick->VAL = 0;                                  	 // clear timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);    	 // clear count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}
