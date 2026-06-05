/*
 *******************************************************************************
 * CAPSTONE: LCD
 *******************************************************************************
 * @file           : LCD.c
 * @brief          : Source file for LCD GPIO control and tuner display updates
 * project         : EE 329 S'26 Capstone
 * authors         : Mick Barnett, Tyler Ragasa
 * version         : 0.1
 * date            : 06/2026
 * compiler        : STM32CubeIDE
 * target          : NUCLEO-L4A6ZG
 * clocks          : LCD timing delays utilize SysTick via delay_us()
 * @attention      : SysTick_Init() must be called before LCD_init()
 *                   LCD_Config() should be called before LCD_init()
 *******************************************************************************
 */
#include "LCD.h"
/*
 * Writes 4-bit nibble to LCD data pins
 * SInce LCD DBs aren't wired in sequential order, so this function
 * ensures each bit of nibble goes to the appropriate GPIO port
 */
static void LCD_put_nibble(uint8_t nibble)
{
   /* Clear only LCD data pins */
   LCD_PORT->ODR &= ~LCD_DATA_BITS;
   /* Map nibble bits onto scrambled GPIO pins */
   if (nibble & 0x01)
      LCD_PORT->ODR |= LCD_DB4; // DB4
   if (nibble & 0x02)
      LCD_PORT->ODR |= LCD_DB5; // DB5
   if (nibble & 0x04)
      LCD_PORT->ODR |= LCD_DB6; // DB6
   if (nibble & 0x08)
      LCD_PORT->ODR |= LCD_DB7; // DB7
}
/*---------------------------------------------------------------*/
/*
 * Configures GPIO pins used by LCD (Data and control pins)
 * - Sets all pins as output, push-pull, no pupd
 * - Clears LCD pins for ODR
 */
void LCD_Config(void)
{
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
   LCD_PORT->MODER &= ~((3 << (LCD_DB4_PIN * 2)) | (3 << (LCD_DB5_PIN * 2)) |
                        (3 << (LCD_DB6_PIN * 2)) | (3 << (LCD_DB7_PIN * 2)) |
                        (3 << (LCD_RS_PIN * 2)) | (3 << (LCD_EN_PIN * 2)));
   LCD_PORT->MODER |= ((1 << (LCD_DB4_PIN * 2)) | (1 << (LCD_DB5_PIN * 2)) |
                       (1 << (LCD_DB6_PIN * 2)) | (1 << (LCD_DB7_PIN * 2)) |
                       (1 << (LCD_RS_PIN * 2)) | (1 << (LCD_EN_PIN * 2)));
   LCD_PORT->OTYPER &= ~(LCD_DB4 | LCD_DB5 | LCD_DB6 |
                         LCD_DB7 | LCD_RS | LCD_EN);
   LCD_PORT->OSPEEDR &= ~((3 << (LCD_DB4_PIN * 2)) | (3 << (LCD_DB5_PIN * 2)) |
                          (3 << (LCD_DB6_PIN * 2)) | (3 << (LCD_DB7_PIN * 2)) |
                          (3 << (LCD_RS_PIN * 2)) | (3 << (LCD_EN_PIN * 2)));
   LCD_PORT->PUPDR &= ~((3 << (LCD_DB4_PIN * 2)) | (3 << (LCD_DB5_PIN * 2)) |
                        (3 << (LCD_DB6_PIN * 2)) | (3 << (LCD_DB7_PIN * 2)) |
                        (3 << (LCD_RS_PIN * 2)) | (3 << (LCD_EN_PIN * 2)));
   LCD_PORT->ODR &= ~(LCD_DATA_BITS | LCD_RS | LCD_EN);
}
/*
 * Helper function: Pulses LCD Enable
 * Necessary for LCD to latch command / data nibble
 */
void LCD_pulse_ENA(void)
{
   /* LCD latches on falling edge of E */
   LCD_PORT->ODR |= LCD_EN;
   delay_us(5);
   LCD_PORT->ODR &= ~LCD_EN;
   delay_us(5);
}
/*
 * Used to send only upper nibble of a command
 * - Utilized for LCD wake-up sequence in LCD_init();
 * - Clears RS so LCD interprets DB as command mode
 */
void LCD_4b_command(uint8_t command)
{
   LCD_PORT->ODR &= ~LCD_RS;              // RS = 0 for command
   LCD_put_nibble((command >> 4) & 0x0F); // send high nibble only
   delay_us(5);
   LCD_pulse_ENA();
}
/*
 * Sends 8-bit LCD commends in 4-bit mode
 * - Sends upper and lower nibble of command seperately to data lines
 * - Enables / latches LCD data
 * - Safety measure included for "slow commands" that require a longer delay
 */
void LCD_command(uint8_t command)
{
   LCD_PORT->ODR &= ~LCD_RS;              // RS = 0 for command
   LCD_put_nibble((command >> 4) & 0x0F); // high nibble
   delay_us(5);
   LCD_pulse_ENA();
   LCD_put_nibble(command & 0x0F); // low nibble
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
/*
 * Write a single character to the LCD
 * - Sets RS so LCD interprets DB in data mode
 * - 8-bit char sent as 2, 4-bit nibbles
 */
void LCD_write_char(uint8_t letter)
{
   LCD_PORT->ODR |= LCD_RS;              // RS = 1 for data
   LCD_put_nibble((letter >> 4) & 0x0F); // high nibble
   delay_us(5);
   LCD_pulse_ENA();
   LCD_put_nibble(letter & 0x0F); // low nibble
   delay_us(5);
   LCD_pulse_ENA();
   LCD_PORT->ODR &= ~LCD_RS; // restore RS low
   delay_us(40);
}
/* LCD accepts bitmask of null-terminated string inputs.
 * - Repeatedly call char_write to send a full line of text at once
 */
void LCD_write_string(char *string)
{
   while (*string != '\0')
   {
      LCD_write_char((uint8_t)(*string));
      string++;
   }
}
/*---------------------------------------------------------------*/
/*
 * Moves LCD cursor to desired row and column
 * - 2 rows, 16 cols.
 * - Writing to LCD prints first char at location then shifts 1 right
 */
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
/* 
* Completely reset LCD screen using preset LCD command
*/
void LCD_clear(void)
{
   LCD_command(0x01);
}
/* Initialize the LCD screen by first, waking it up, setting it to 4 bit mode
 * - Writes default "home screen" with static labels for each line
 */
void LCD_init(void)
{
   delay_us(40000); // wait > 40 ms after power-up
   LCD_4b_command(0x30);
   delay_us(5000);
   LCD_4b_command(0x30);
   delay_us(200);
   LCD_4b_command(0x30);
   delay_us(200);
   LCD_4b_command(0x20);
   delay_us(40);
   /* Initialization sequence */
   LCD_command(0x28); // 4-bit, 2-line, 5x8 font
   LCD_command(0x0C); // display on, cursor off, blink off
   LCD_command(0x06); // entry mode: increment, no shift
   LCD_command(0x01); // clear display
   /* Home screen */
   LCD_set_cursor(0, 0);
   LCD_write_string("Note: -- |b x #|");
   LCD_set_cursor(1, 0);
   LCD_write_string("Cents:");
}
/*
* Updates the LCD with current note and cents tuning error
* - Only overwrite changing data fields, leaves static LCD home screen
* - Reads global "note" and "cents" values updated by processor module
* - Determines whether current note is flat/sharp/in-tune relative to target
* - Line 1: Displays/updates target note
* - Line 1: Updates tune indicator pointing towards flat 'b' or sharp '#'
* - Converts cents float to display-ready whole and decimal integers
* - Line 2: Clears then updates cents error
*/
void LCD_show_data(void)
{
   char cents_str[12];
   char tune_indicator = 'x';
   // Determine visual indicator for tune accuracy
   // Within +/- 5 cents = in tune
   if (cents < -2.0f)
   {
      tune_indicator = '<';
   }
   else if (cents > 2.0f)
   {
      tune_indicator = '>';
   }
   else
   {
      tune_indicator = 'x';
   }
   // Line 1: "Note: -- |b x #|"
   // Update note '--'
   LCD_set_cursor(0, 6);
   LCD_write_string("  "); // clear old note
   LCD_set_cursor(0, 6);
   LCD_write_string(notes[note].name);
   // Update tune indicator
   LCD_set_cursor(0, 12);
   LCD_write_char(tune_indicator);
   // Convert float cents to str W.D
   int whole = (int)cents;
   int decimal = (int)(fabsf(cents * 10.0f)) % 10;
   snprintf(cents_str, sizeof(cents_str), "%d.%d", whole, decimal);
   // Line 2: "Error/Cents:-X.X"
   // Update cents
   LCD_set_cursor(1, 7);
   LCD_write_string("          "); // clear old value
   LCD_set_cursor(1, 7);
   LCD_write_string(cents_str);
}
