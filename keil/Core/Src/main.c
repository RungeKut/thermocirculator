#include <main.h>
#include <gd32e10_it.h>
#include <gd32e10x_libopt.h>
#include <stdio.h>
#include <gd32e10x.h>
#include <systick.h>
#include <adc.h>
#include <gpio.h>
#include <rtc.h>
#include <tim.h>

int main(void)
{
	uint32_t time;
  SystemInit();
	HAL_InitTick(1000);
	HAL_Delay(500);
	GPIO_Init();
  RTC_Init();
  ADC_Init();
  TIM1_Init();
	
  while(1)
  {
		HAL_Delay(2000);
		
time = rtc_counter_get(); // Получаем текущее значение счетчика (в секундах)
		
		HAL_Delay(time);
        //gpio_bit_set(GPIOA,GPIO_PIN_15);
        //delay_1ms(1000);
        //gpio_bit_reset(GPIOA,GPIO_PIN_15);
        //delay_1ms(1000);
  }
}
