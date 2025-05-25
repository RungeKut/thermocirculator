#include <tim.h>
#include "gd32e10x.h"
#include <gd32e10_it.h>

#define HAL_MAX_DELAY 0xFFFFFFFFU

__attribute__((weak)) void HAL_Delay(uint32_t Delay) {
    uint32_t tickstart = uwTick;
    uint32_t wait = Delay;

    if (wait < HAL_MAX_DELAY) {
        wait += 1U;  // Минимальная гарантия ожидания
    }

    while ((uwTick - tickstart) < wait) {
        // Ждём
    }
}

void HAL_InitTick(uint32_t TickFreq) {
    // Вычисляем количество тактов на один тик (зависит от частоты ядра)
    uint32_t sysTickInterval = SystemCoreClock / TickFreq;

    // Настраиваем SysTick
    SysTick_Config(sysTickInterval);

    // Можно также настроить приоритет прерывания
    NVIC_SetPriority(SysTick_IRQn, 0);
}

void TIM1_Init(void)
{
		gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_15);
	  gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_3);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_10);
	  gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_11);
	
		timer_oc_parameter_struct timer_ocinitpara;
		timer_parameter_struct timer_initpara;
		
		rcu_periph_clock_enable(RCU_TIMER1);
		
		timer_deinit(TIMER1);
		
		/* initialize TIMER init parameter struct */
		timer_struct_para_init(&timer_initpara);
		/* TIMER1 configuration */
		timer_initpara.prescaler         = 0x0000;
		timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
		timer_initpara.counterdirection  = TIMER_COUNTER_UP;
		timer_initpara.period            = 0xFFFF;
		timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
		timer_initpara.repetitioncounter = 0;
		
		timer_init(TIMER1, &timer_initpara);
		
		/* initialize TIMER channel output parameter struct */
		timer_channel_output_struct_para_init(&timer_ocinitpara);
		/* CH0, CH1 and CH2 configuration in PWM mode */
		timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
		timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
		timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
		timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
		timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
		timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
		
		timer_channel_output_config(TIMER1, TIMER_CH_0, &timer_ocinitpara);
		timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocinitpara);
		timer_channel_output_config(TIMER1, TIMER_CH_2, &timer_ocinitpara);
		timer_channel_output_config(TIMER1, TIMER_CH_3, &timer_ocinitpara);
	
		/* CH0 configuration in PWM mode0, duty cycle 25% */
		timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, 4000);
		timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM0);
		timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
	
		/* CH1 configuration in PWM mode0, duty cycle 50% */
		timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 8000);
		timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
		timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
	
		/* CH2 configuration in PWM mode0, duty cycle 75% */
		timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, 12000);
		timer_channel_output_mode_config(TIMER1, TIMER_CH_2, TIMER_OC_MODE_PWM0);
		timer_channel_output_shadow_config(TIMER1, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);
		
		/* CH3 configuration in PWM mode0, duty cycle 75% */
		timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_3, 12000);
		timer_channel_output_mode_config(TIMER1, TIMER_CH_3, TIMER_OC_MODE_PWM0);
		timer_channel_output_shadow_config(TIMER1, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);
		
	  //gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP, DISABLE);
		gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP, ENABLE);
		
		/* auto-reload preload enable */
		timer_auto_reload_shadow_enable(TIMER1);
		/* auto-reload preload enable */
		timer_enable(TIMER1);
}

void TIM2_Init(void)
{
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_15);
		timer_oc_parameter_struct timer_ocinitpara;
		timer_parameter_struct timer_initpara;
		
		rcu_periph_clock_enable(RCU_TIMER2);
		
		timer_deinit(TIMER2);
		
		/* initialize TIMER init parameter struct */
		timer_struct_para_init(&timer_initpara);
		/* TIMER2 configuration */
		timer_initpara.prescaler         = 0x0000;
		timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
		timer_initpara.counterdirection  = TIMER_COUNTER_UP;
		timer_initpara.period            = 0xFFFF;
		timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
		timer_initpara.repetitioncounter = 0;
		
		timer_init(TIMER2, &timer_initpara);
		
		/* initialize TIMER channel output parameter struct */
		timer_channel_output_struct_para_init(&timer_ocinitpara);
		/* CH0, CH1 and CH2 configuration in PWM mode */
		timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
		timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
		timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
		timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
		timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
		timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
		
		timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_ocinitpara);
		//timer_channel_output_config(TIMER2, TIMER_CH_1, &timer_ocinitpara);
		//timer_channel_output_config(TIMER2, TIMER_CH_2, &timer_ocinitpara);
		//timer_channel_output_config(TIMER2, TIMER_CH_3, &timer_ocinitpara);
	
		/* CH0 configuration in PWM mode0, duty cycle 25% */
		timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 4000);
		timer_channel_output_mode_config(TIMER2, TIMER_CH_0, TIMER_OC_MODE_PWM0);
		timer_channel_output_shadow_config(TIMER2, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
	
		/* CH1 configuration in PWM mode0, duty cycle 50% */
		//timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, 8000);
		//timer_channel_output_mode_config(TIMER2, TIMER_CH_1, TIMER_OC_MODE_PWM0);
		//timer_channel_output_shadow_config(TIMER2, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
	
		/* CH2 configuration in PWM mode0, duty cycle 75% */
		//timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_2, 12000);
		//timer_channel_output_mode_config(TIMER2, TIMER_CH_2, TIMER_OC_MODE_PWM0);
		//timer_channel_output_shadow_config(TIMER2, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);
		
		/* CH3 configuration in PWM mode0, duty cycle 75% */
		//timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_3, 12000);
		//timer_channel_output_mode_config(TIMER2, TIMER_CH_3, TIMER_OC_MODE_PWM0);
		//timer_channel_output_shadow_config(TIMER2, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);
	
		gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP, ENABLE);
		gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_15);
		/* auto-reload preload enable */
		timer_auto_reload_shadow_enable(TIMER2);
		/* auto-reload preload enable */
		timer_enable(TIMER2);
}
