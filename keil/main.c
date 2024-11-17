#include <gd32e10x_libopt.h>
#include <stdio.h>
#include "gd32e10x.h"
#include "systick.h"

int main(void)
{
  SystemInit();
	//rcu_periph_clock_enable(RCU_BASE);
  rcu_periph_clock_enable(RCU_GPIOA);
  gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_10);

  //gpio_bit_write(GPIOA, GPIO_PIN_10, 1);
  while(1)
  {
        gpio_bit_set(GPIOA,GPIO_PIN_10);
        //delay_1ms(1000);
        gpio_bit_reset(GPIOA,GPIO_PIN_10);
        //delay_1ms(1000);
  }
}
