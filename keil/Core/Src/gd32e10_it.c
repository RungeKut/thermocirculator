#include <gd32e10_it.h>

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

volatile uint32_t uwTick = 0;

void SysTick_Handler(void) {
    uwTick++;
}

void RTC_IRQHandler(void)
{
}

void ADC0_1_IRQHandler(void)
{
  //HAL_ADC_IRQHandler(&hadc);
}

void TIMER2_IRQHandler(void)
{
  if ( timer_interrupt_flag_get(TIMER2 , TIMER_INT_UP) != RESET ) 
	{
		timer_interrupt_flag_clear(TIMER2 , TIMER_INT_UP);
	}
}
