/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include "main.h"

void SystemClock_Config(void);

#define DEBUG_MODE 0   // Set to 1 to debug using LPUART
#define MEAS_LATENCY 0 // Set to 1 when want to meas. update latency
static uint16_t sample_window[ADC_WND_SIZE];

int main(void)
{
   /*===== System and module init/config =====*/
   // System Defaults
   HAL_Init();
   SystemClock_Config();
   SysTick_Init();
   // User Defined Inits
   ADC_init(); // Config ADC & IRQ but doesnt start conv.
   FFT_Init();
   Processor_init();
   if (DEBUG_MODE)
   { // LPUART set up for troubleshooting
      LPUART_init();
      LPUART_clear_screen();
      LPUART_print("FFT troubleshooting started...\r\n");
   }
   else
   {
      LCD_Config();
      LCD_init();
   }
   if (MEAS_LATENCY)
   { // GPIO Config for measuring latency
      RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
      GPIOA->MODER &= ~GPIO_MODER_MODE0;
      GPIOA->MODER |= GPIO_MODER_MODE0_0; // output
      GPIOA->OTYPER &= ~GPIO_OTYPER_OT0; // push-pull
      GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED0;
      GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0; // v high speed
      GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0; // no pupd
      GPIOA->BRR = GPIO_PIN_3; // start low
   }
   TIM3_init(); // Starts 8 kHz timer for ADC conv.

   /*===== Main process flow control =====*/
   while (1)
   {
      if (ADC_wndw_ready) // Global flag set by ADC.c IRQHandler
      {
         if (MEAS_LATENCY)
         {
            ADC_wndw_ready = 0; // Clear flag -> discard window
            GPIOA->BSRR = GPIO_PIN_3; // Toggle high
            while (!ADC_wndw_ready)
            {
               ; // Wait for next window to be ready
            }
         }
         /*----- ADC -----*/
         // Copy Sample Window array of raw counts
         ADC_GetWindow(sample_window);
         /*----- FFT -----*/
         // Processes samples into freq and returns bin freq?
         float freq = FFT_Get_Freq(sample_window, ADC_WND_SIZE);
         /*----- PROCESSING -----*/
         Processor(freq); // Updates globals: note, cents, percent_error
         /*----- DISPLAY -----*/
         if (DEBUG_MODE)
         {
            float mag = FFT_Get_Last_Magnitude();
            uint16_t bin = FFT_Get_Last_Peak_Bin();
            LPUART_print_FFT_debug(bin, freq, mag);
         }
         else
         {
            LCD_show_data(); // Assumes globals have been updated
            if (MEAS_LATENCY)
            {
               GPIOB->BRR = GPIO_PIN_0; // GPIO LOW
            }
         }
      }
   }
}

void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

   if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
   {
      Error_Handler();
   }

   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
   RCC_OscInitStruct.MSIState = RCC_MSI_ON;
   RCC_OscInitStruct.MSICalibrationValue = 0;
   RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }

   RCC_ClkInitStruct.ClockType =
       RCC_CLOCKTYPE_HCLK |
       RCC_CLOCKTYPE_SYSCLK |
       RCC_CLOCKTYPE_PCLK1 |
       RCC_CLOCKTYPE_PCLK2;

   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                           FLASH_LATENCY_0) != HAL_OK)
   {
      Error_Handler();
   }
}

void Error_Handler(void)
{
   /* USER CODE BEGIN Error_Handler_Debug */
   /* User can add his own implementation to report the HAL error return state */
   __disable_irq();
   while (1)
   {
   }
   /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
   /* USER CODE BEGIN 6 */
   /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
   /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
