
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "delay.h"
#include "ADC.h"
#include "FFT.h"
#include "Processor.h"
#include "TIMER.h"
#include "lpuart.h"
#include "LCD.h"

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
