#include <gpio.h>

void GPIO_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_AF);
	
	//SWD (Serial Wire Debug) остаётся включённым, но JTAG отключается.
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
  
	gpio_init(WATER_LEVEL_LOW_GPIO_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,
		WATER_LEVEL_LOW_Pin);
	
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,
		WATER_LEVEL_HIGH_Pin|START_BUTTON_Pin|STOP_BUTTON_Pin|MENU_BUTTON_Pin
		|LEFT_BUTTON_Pin);
	
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,
		RIGHT_BUTTON_Pin|GPIO_PIN_15);
	
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ,
		AC_MOTOR_Pin|HEATER_Pin|DISP_DB2_Pin|LED_R_Pin
    |DISP_RS_Pin|DISP_DB3_Pin|DISP_DB4_Pin|DISP_DB5_Pin
    |DISP_DB7_Pin|DISP_DB6_Pin);
	
	gpio_init(GPIOB, GPIO_MODE_IPD, GPIO_OSPEED_2MHZ,
		GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_6|GPIO_PIN_7);
		
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ,
		COOLER_Pin|DC_MOTOR_Pin|BACKLIGHT_Pin|DISP_E_Pin
    |DISP_RW_Pin);
	
  /*Configure GPIO pin Output Level */
  gpio_bit_reset(GPIOB, AC_MOTOR_Pin|HEATER_Pin|DISP_DB2_Pin|LED_R_Pin
                          |DISP_RS_Pin|DISP_DB3_Pin|DISP_DB4_Pin|DISP_DB5_Pin
                          |DISP_DB7_Pin|DISP_DB6_Pin);

  /*Configure GPIO pin Output Level */
  gpio_bit_reset(GPIOA, COOLER_Pin|DC_MOTOR_Pin|BACKLIGHT_Pin|DISP_E_Pin
                          |DISP_RW_Pin);

	//gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
	//gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
	//gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
  //gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	//gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
}
