#include "gd32e10x.h"
#include "systick.h"

volatile static uint32_t delay;

void systick_config(void)
{
    // Настройка SysTick на 1 мс
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay_ms(uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        // Ожидание, пока флаг COUNTFLAG не станет равен 1
        while (!((SysTick->CTRL) & SysTick_CTRL_COUNTFLAG_Msk));
    }

    // Отключение SysTick
    SysTick->CTRL = 0;
}

void delay_1ms(uint32_t count)
{
    delay = count;


	while(0U != delay){
    }
}

void delay_decrement(void)
{
    if (0U != delay){
        delay--;
    }
}
