#include "main.h"

// Initiallize / Config
TIM3_init();
ADC_init();

// draft adc utilization
uint16_t Sample_Window[ADC_WND_SiZE] = {0};
while(1){
   if (ADC_wndw_ready) {
      ADC_GetWindow(Sample_Window);
      // FFT_Function(Sample_Window);
   }
}