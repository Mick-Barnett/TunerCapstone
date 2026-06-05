#include "lpuart.h"

/*----- Local Defines -----*/
#define LPUART_BAUD 115200U
#define LPUART_BRR ((256*SystemCoreClock)/LPUART_BAUD)

static void LPUART_print_char(char c)
{
    while (!(LPUART1->ISR & USART_ISR_TXE)) { }
    LPUART1->TDR = c;
}

void LPUART_init(void)
{
   PWR->CR2  |=  PWR_CR2_IOSV;
   RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOGEN;
   RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;

   GPIOG->MODER   &= ~((3U << (7*2)) | (3U << (8*2)));
   GPIOG->MODER   |=  ((2U << (7*2)) | (2U << (8*2)));
   GPIOG->OTYPER  &= ~((1U << 7) | (1U << 8));
   GPIOG->OSPEEDR &= ~((3U << (7*2)) | (3U << (8*2)));
   GPIOG->OSPEEDR |=  ((3U << (7*2)) | (3U << (8*2)));
   GPIOG->PUPDR   &= ~((3U << (7*2)) | (3U << (8*2)));
   GPIOG->PUPDR   |=  ((1U << (7*2)) | (1U << (8*2)));
   GPIOG->AFR[0]  &= ~(0xFU << (7*4));
   GPIOG->AFR[0]  |=  (8U   << (7*4));
   GPIOG->AFR[1]  &= ~(0xFU << ((8-8)*4));
   GPIOG->AFR[1]  |=  (8U   << ((8-8)*4));

   LPUART1->CR1 &= ~USART_CR1_UE;
   LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);
   LPUART1->CR2 &= ~USART_CR2_STOP;
   LPUART1->CR1 &= ~USART_CR1_PCE;
   LPUART1->BRR  =  LPUART_BRR;
   LPUART1->CR1 |=  USART_CR1_TE | USART_CR1_RE;
   LPUART1->CR1 |=  USART_CR1_UE;
}

void LPUART_print(const char *message)
{
    uint16_t i = 0;

    while (message[i] != '\0')
    {
        LPUART_print_char(message[i]);
        i++;
    }
}

void LPUART_clear_screen(void)
{
    LPUART_print("\033[2J");
    LPUART_print("\033[H");
}

void LPUART_cursor_home(void)
{
    LPUART_print("\033[H");
}

void LPUART_print_uint32(uint32_t value)
{
    char buffer[11];
    uint8_t i = 0;

    if (value == 0)
    {
        LPUART_print_char('0');
        return;
    }

    while (value > 0 && i < sizeof(buffer))
    {
        buffer[i++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (i > 0)
    {
        LPUART_print_char(buffer[--i]);
    }
}

void LPUART_print_float_2(float value)
{
    if (value < 0.0f)
    {
        LPUART_print_char('-');
        value = -value;
    }

    uint32_t whole = (uint32_t)value;
    uint32_t frac =
        (uint32_t)((value - (float)whole) * 100.0f + 0.5f);

    if (frac >= 100)
    {
        whole++;
        frac -= 100;
    }

    LPUART_print_uint32(whole);
    LPUART_print_char('.');
    LPUART_print_char((char)('0' + (frac / 10)));
    LPUART_print_char((char)('0' + (frac % 10)));
}

void LPUART_print_FFT_debug(uint16_t peak_bin,
                            float peak_freq,
                            float peak_mag)
{
    LPUART_cursor_home();

    LPUART_print("========== FFT DEBUG ==========\r\n\r\n");

    LPUART_print("Peak Bin      : ");
    LPUART_print_uint32(peak_bin);
    LPUART_print("\r\n");

    LPUART_print("Peak Frequency: ");
    LPUART_print_float_2(peak_freq);
    LPUART_print(" Hz\r\n");

    LPUART_print("Peak Magnitude: ");
    LPUART_print_float_2(peak_mag);
    LPUART_print("\r\n");

    LPUART_print("\r\n");
}

void LPUART_print_FFT_two_peaks(uint16_t bin1,
                                float freq1,
                                float mag1,
                                uint16_t bin2,
                                float freq2,
                                float mag2)
{
    LPUART_cursor_home();

    LPUART_print("========== FFT TWO PEAK DEBUG ==========\r\n\r\n");

    LPUART_print("Peak 1 Bin : ");
    LPUART_print_uint32(bin1);
    LPUART_print("\r\n");

    LPUART_print("Peak 1 Freq: ");
    LPUART_print_float_2(freq1);
    LPUART_print(" Hz\r\n");

    LPUART_print("Peak 1 Mag : ");
    LPUART_print_float_2(mag1);
    LPUART_print("\r\n\r\n");

    LPUART_print("Peak 2 Bin : ");
    LPUART_print_uint32(bin2);
    LPUART_print("\r\n");

    LPUART_print("Peak 2 Freq: ");
    LPUART_print_float_2(freq2);
    LPUART_print(" Hz\r\n");

    LPUART_print("Peak 2 Mag : ");
    LPUART_print_float_2(mag2);
    LPUART_print("\r\n");
}

char LPUART_read_char(void)
{
    while (!(LPUART1->ISR & USART_ISR_RXNE)) { }

    char c = (char)(LPUART1->RDR & 0xFF);

    if (c == '\n')
    {
        c = '\r';
    }

    return c;
}

