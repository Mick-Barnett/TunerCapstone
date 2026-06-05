/*
 * LCD.h
 *
 *  Created on: Jun 1, 2026
 *      Author: mickp
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include "stm32l4xx_hal.h"
#include "delay.h"
/*------------------------ PORT DEFINITIONS ------------------------*/
#define LCD_PORT        GPIOC
/*------------------------ LCD CONTROL PINS ------------------------*/
#define LCD_RS_PIN      11
#define LCD_EN_PIN      12
#define LCD_RS          GPIO_PIN_11
#define LCD_EN          GPIO_PIN_12
/*------------------------ LCD DATA PINS ---------------------------*/
/*
* Scrambled wiring:
* DB4 -> PC4
* DB5 -> PC1
* DB6 -> PC3
* DB7 -> PC0
*/
#define LCD_DB4_PIN     4
#define LCD_DB5_PIN     1
#define LCD_DB6_PIN     3
#define LCD_DB7_PIN     0
#define LCD_DB4         GPIO_PIN_4
#define LCD_DB5         GPIO_PIN_1
#define LCD_DB6         GPIO_PIN_3
#define LCD_DB7         GPIO_PIN_0
#define LCD_DATA_BITS   (LCD_DB4 | LCD_DB5 | LCD_DB6 | LCD_DB7)
/*------------------------ FUNCTION PROTOTYPES ---------------------*/
void LCD_Config(void);
void LCD_init(void);
void LCD_show_data(void);

void LCD_pulse_ENA(void);
void LCD_4b_command(uint8_t command);
void LCD_command(uint8_t command);
void LCD_write_char(uint8_t letter);
void LCD_write_string(char *string);
void LCD_set_cursor(uint8_t row, uint8_t col);
void LCD_clear(void);
#endif /* INC_LCD_H_ */

