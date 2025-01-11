#include <gd32e10x_libopt.h>
#include <stdio.h>
#include "gd32e10x.h"
#include "systick.h"

int main(void)
{
  SystemInit();
  rcu_periph_clock_enable(RCU_GPIOA);
  gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_10);
	
	rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);
	timer_parameter_struct timer_initpara;
	/* TIMER1 configuration */
timer_initpara.prescaler         = 0xFFFF;
timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
timer_initpara.counterdirection  = TIMER_COUNTER_UP;
timer_initpara.period            = 0xFFFF;
timer_initpara.clockdivision     = TIMER_CKDIV_DIV4;
timer_initpara.repetitioncounter = 0;
timer_init(TIMER1, &timer_initpara);
	timer_interrupt_enable(TIMER1,TIMER_INT_UP);
	nvic_irq_enable(TIMER1_IRQn, 0, 3);
	timer_enable(TIMER1);
	//rcu_periph_clock_disable(RCU_TIMER1);/*Turn off and wait for use*/ 
  while(1)
  {
        gpio_bit_set(GPIOA,GPIO_PIN_10);
        //delay_1ms(1000);
        gpio_bit_reset(GPIOA,GPIO_PIN_10);
        //delay_1ms(1000);
  }
}

void TIMER1_IRQHandler (void);

void TIMER1_IRQHandler (void)
{
	if ( timer_interrupt_flag_get(TIMER1 , TIMER_INT_UP) != RESET ) 
	{
		timer_interrupt_flag_clear(TIMER1 , TIMER_INT_UP);
	}
}
