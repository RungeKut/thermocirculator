#include <rtc.h>
#include "gd32e10x.h"
#include "gd32e10x_rcu.h"
#include "gd32e10x_pmu.h"
#include "gd32e10x_rtc.h"

void rtc_deinit(void) {
    RTC_CTL |= RTC_CTL_LWOFF; // Ждём завершения прошлой записи
    RTC_CNTL = 0;
    RTC_CNTH = 0;
}

void RTC_Init(void)
{
	  /* Включаем PMU и разрешаем доступ к BKP домену */
    rcu_periph_clock_enable(RCU_PMU);   // без ENABLE!
    pmu_backup_write_enable();

    /* Включаем LSE (внешний кварц 32.768 кГц) */
    rcu_osci_on(RCU_LXTAL);
    while (!rcu_flag_get(RCU_FLAG_LXTALSTB)) {
        // Ждём готовности LSE
    }

    /* Выбираем LSE как источник тактирования RTC */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    /* Включаем тактирование RTC */
    rcu_periph_clock_enable(RCU_RTC);  // без ENABLE!

    /* Сброс RTC регистров */
    rtc_deinit();

    /* Входим в режим конфигурации */
    rtc_configuration_mode_enter();

    /* Устанавливаем предделитель: 32767 = 32.768 кГц / 32768 = 1 Гц */
    rtc_prescaler_set(32767);

    /* Выходим из режима конфигурации */
    rtc_configuration_mode_exit();

    /* Включаем прерывание по будильнику */
    rtc_interrupt_enable(RTC_INT_ALARM);

    /* Разрешаем прерывание в NVIC */
    nvic_irq_enable(RTC_ALARM_IRQn, 0, 0);
}
