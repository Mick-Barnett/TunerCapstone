


#include "LCD.h"
#include <stdio.h>  // for snprintf to handle strings
#include <math.h>
#include "Processor.h"

static void LCD_put_nibble(uint8_t nibble)
{
   /* Clear only LCD data pins */
   LCD_PORT->ODR &= ~LCD_DATA_BITS;
   /* Map nibble bits onto scrambled GPIO pins */
   if (nibble & 0x01) LCD_PORT->ODR |= LCD_DB4;   // DB4
   if (nibble & 0x02) LCD_PORT->ODR |= LCD_DB5;   // DB5
   if (nibble & 0x04) LCD_PORT->ODR |= LCD_DB6;   // DB6
   if (nibble & 0x08) LCD_PORT->ODR |= LCD_DB7;   // DB7
}
/*---------------------------------------------------------------*/
void LED_Config(void)
{
   RCC->AHB2ENR   |=  RCC_AHB2ENR_GPIOBEN;
   GPIOB->MODER   &= ~((3 << (LED_PIN_1*2)) | (3 << (LED_PIN_2*2)));
   GPIOB->MODER   |=  ((1 << (LED_PIN_1*2)) | (1 << (LED_PIN_2*2)));
   GPIOB->OTYPER  &= ~((3 << (LED_PIN_1)) | (3 << (LED_PIN_2)));
   GPIOB->OSPEEDR &= ~((3 << (LED_PIN_1*2)) | (3 << (LED_PIN_2*2)));
   GPIOB->PUPDR   &= ~((3 << (LED_PIN_1*2)) | (3 << (LED_PIN_2*2)));
   GPIOB->BRR      =  (GPIO_PIN_2 | GPIO_PIN_6);
}
void LCD_Config(void)
{
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
   LCD_PORT->MODER &= ~((3 << (LCD_DB4_PIN * 2)) | (3 << (LCD_DB5_PIN * 2)) |
                        (3 << (LCD_DB6_PIN * 2)) | (3 << (LCD_DB7_PIN * 2)) |
                        (3 << (LCD_RS_PIN  * 2)) | (3 << (LCD_EN_PIN  * 2)));
   LCD_PORT->MODER |=  ((1 << (LCD_DB4_PIN * 2)) | (1 << (LCD_DB5_PIN * 2)) |
                        (1 << (LCD_DB6_PIN * 2)) | (1 << (LCD_DB7_PIN * 2)) |
                        (1 << (LCD_RS_PIN  * 2)) | (1 << (LCD_EN_PIN  * 2)));
   LCD_PORT->OTYPER &= ~(LCD_DB4 | LCD_DB5 | LCD_DB6 |
                         LCD_DB7 | LCD_RS  | LCD_EN);
   LCD_PORT->OSPEEDR &= ~((3 << (LCD_DB4_PIN * 2)) | (3 << (LCD_DB5_PIN * 2)) |
                          (3 << (LCD_DB6_PIN * 2)) | (3 << (LCD_DB7_PIN * 2)) |
                          (3 << (LCD_RS_PIN  * 2)) | (3 << (LCD_EN_PIN  * 2)));
   LCD_PORT->PUPDR &= ~((3 << (LCD_DB4_PIN * 2)) | (3 << (LCD_DB5_PIN * 2)) |
                        (3 << (LCD_DB6_PIN * 2)) | (3 << (LCD_DB7_PIN * 2)) |
                        (3 << (LCD_RS_PIN  * 2)) | (3 << (LCD_EN_PIN  * 2)));
   LCD_PORT->ODR &= ~(LCD_DATA_BITS | LCD_RS | LCD_EN);
}
void LCD_pulse_ENA(void)
{
   /* LCD latches on falling edge of E */
   LCD_PORT->ODR |= LCD_EN;
   delay_us(5);
   LCD_PORT->ODR &= ~LCD_EN;
   delay_us(5);
}
void LCD_4b_command(uint8_t command)
{
   LCD_PORT->ODR &= ~LCD_RS;                   // RS = 0 for command
   LCD_put_nibble((command >> 4) & 0x0F);      // send high nibble only
   delay_us(5);
   LCD_pulse_ENA();
}
void LCD_command(uint8_t command)
{
   LCD_PORT->ODR &= ~LCD_RS;                   // RS = 0 for command
   LCD_put_nibble((command >> 4) & 0x0F);      // high nibble
   delay_us(5);
   LCD_pulse_ENA();
   LCD_put_nibble(command & 0x0F);             // low nibble
   delay_us(5);
   LCD_pulse_ENA();
   /* Long delay for slow commands */
   if ((command == 0x01) || (command == 0x02))
   {
       delay_us(2000);
   }
   else
   {
       delay_us(40);
   }
}
/*---------------------------------------------------------------*/
void LCD_write_char(uint8_t letter)
{
   LCD_PORT->ODR |= LCD_RS;                    // RS = 1 for data
   LCD_put_nibble((letter >> 4) & 0x0F);       // high nibble
   delay_us(5);
   LCD_pulse_ENA();
   LCD_put_nibble(letter & 0x0F);              // low nibble
   delay_us(5);
   LCD_pulse_ENA();
   LCD_PORT->ODR &= ~LCD_RS;                   // restore RS low
   delay_us(40);
}
/*LCD accepts bitmask of string inputs. repeatedly call char_write to
* make a full line of text at once */
void LCD_write_string(char *string)
{
   while (*string != '\0')
   {
       LCD_write_char((uint8_t)(*string));
       string++;
   }
}
/*---------------------------------------------------------------*/
void LCD_set_cursor(uint8_t row, uint8_t col)
{
   uint8_t address;
   if (row == 0)
   {
       address = 0x00 + col;
   }
   else
   {
       address = 0x40 + col;
   }
   LCD_command(0x80 | address);
}
/* Completely reset LCD screen */
void LCD_clear(void)
{
   LCD_command(0x01);
}
/* Initialize the LCD screen by first, waking it up, setting it to 4 bit mode,
* and setting the LCD home screen */
void LCD_init(void)
{
   delay_us(40000);    // wait > 40 ms after power-up
   LCD_4b_command(0x30);
   delay_us(5000);
   LCD_4b_command(0x30);
   delay_us(200);
   LCD_4b_command(0x30);
   delay_us(200);
   LCD_4b_command(0x20);
   delay_us(40);
   /* Initialization sequence */
   LCD_command(0x28);  // 4-bit, 2-line, 5x8 font
   LCD_command(0x0C);  // display on, cursor off, blink off
   LCD_command(0x06);  // entry mode: increment, no shift
   LCD_command(0x01);  // clear display
   /* Home screen */
   LCD_set_cursor(0, 0);
   LCD_write_string("Note:");
   LCD_set_cursor(1, 0);
   LCD_write_string("Error/Cents:");
}


void LCD_show_data(void)
{
    char cents_str[12];

    LCD_set_cursor(0, 6);
    LCD_write_string("    ");      // clear old note
    LCD_set_cursor(0, 6);
    LCD_write_string(notes[note].name);

    int whole = (int)cents;
    int decimal = (int)(fabsf(cents * 10.0f)) % 10;

    snprintf(cents_str, sizeof(cents_str), "%d.%d", whole, decimal);

    LCD_set_cursor(1, 13);
    LCD_write_string("      ");    // clear old value
    LCD_set_cursor(1, 13);
    LCD_write_string(cents_str);
}

