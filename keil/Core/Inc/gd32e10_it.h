#ifndef __GD32E10_IT_H
#define __GD32E10_IT_H

#include "main.h"

void Reset_Handler(void); // Reset Handler
void NMI_Handler(void); // NMI Handler
void HardFault_Handler(void); // Hard Fault Handler
void MemManage_Handler(void); // MPU Fault Handler
void BusFault_Handler(void); // Bus Fault Handler
void UsageFault_Handler(void); // Usage Fault Handler
void SVC_Handler(void); // SVCall Handler
void DebugMon_Handler(void); // Debug Monitor Handler
void PendSV_Handler(void); // PendSV Handler
void SysTick_Handler(void); // SysTick Handler
extern volatile uint32_t uwTick;

// external interrupts handler
void WWDGT_IRQHandler(void); // 16:Window Watchdog Timer
void LVD_IRQHandler(void); // 17:LVD through EXTI Line detect
void TAMPER_IRQHandler(void); // 18:Tamper through EXTI Line detect
void RTC_IRQHandler(void); // 19:RTC through EXTI Line
void FMC_IRQHandler(void); // 20:FMC
void RCU_CTC_IRQHandler(void); // 21:RCU and CTC
void EXTI0_IRQHandler(void); // 22:EXTI Line 0
void EXTI1_IRQHandler(void); // 23:EXTI Line 1
void EXTI2_IRQHandler(void); // 24:EXTI Line 2
void EXTI3_IRQHandler(void); // 25:EXTI Line 3
void EXTI4_IRQHandler(void); // 26:EXTI Line 4
void DMA0_Channel0_IRQHandler(void); // 27:DMA0 Channel0
void DMA0_Channel1_IRQHandler(void); // 28:DMA0 Channel1
void DMA0_Channel2_IRQHandler(void); // 29:DMA0 Channel2
void DMA0_Channel3_IRQHandler(void); // 30:DMA0 Channel3
void DMA0_Channel4_IRQHandler(void); // 31:DMA0 Channel4
void DMA0_Channel5_IRQHandler(void); // 32:DMA0 Channel5
void DMA0_Channel6_IRQHandler(void); // 33:DMA0 Channel6
void ADC0_1_IRQHandler(void); // 34:ADC0 and ADC1
void EXTI5_9_IRQHandler(void); // 39:EXTI5 to EXTI9
void TIMER0_BRK_TIMER8_IRQHandler(void); // 40:TIMER0 Break and TIMER8
void TIMER0_UP_TIMER9_IRQHandler(void); // 41:TIMER0 Update and TIMER9
void TIMER0_TRG_CMT_TIMER10_IRQHandler(void); // 42:TIMER0 Trigger and Commutation and TIMER10
void TIMER0_Channel_IRQHandler(void); // 43:TIMER0 Channel Capture Compare
void TIMER1_IRQHandler(void); // 44:TIMER1
void TIMER2_IRQHandler(void); // 45:TIMER2
void TIMER3_IRQHandler(void); // 46:TIMER3
void I2C0_EV_IRQHandler(void); // 47:I2C0 Event
void I2C0_ER_IRQHandler(void); // 48:I2C0 Error
void I2C1_EV_IRQHandler(void); // 49:I2C1 Event
void I2C1_ER_IRQHandler(void); // 50:I2C1 Error
void SPI0_IRQHandler(void); // 51:SPI0
void SPI1_IRQHandler(void); // 52:SPI1
void USART0_IRQHandler(void); // 53:USART0
void USART1_IRQHandler(void); // 54:USART1
void USART2_IRQHandler(void); // 55:USART2
void EXTI10_15_IRQHandler(void); // 56:EXTI10 to EXTI15
void RTC_Alarm_IRQHandler(void); // 57:RTC Alarm
void USBFS_WKUP_IRQHandler(void); // 58:USBFS Wakeup
void TIMER7_BRK_TIMER11_IRQHandler(void); // 59:TIMER7 Break and TIMER11
void TIMER7_UP_TIMER12_IRQHandler(void); // 60:TIMER7 Update and TIMER12
void TIMER7_TRG_CMT_TIMER13_IRQHandler(void); // 61:TIMER7 Trigger and Commutation and TIMER13
void TIMER7_Channel_IRQHandler(void); // 62:TIMER7 Channel Capture Compare
void EXMC_IRQHandler(void); // 64:EXMC
void TIMER4_IRQHandler(void); // 66:TIMER4
void SPI2_IRQHandler(void); // 67:SPI2
void UART3_IRQHandler(void); // 68:UART3
void UART4_IRQHandler(void); // 69:UART4
void TIMER5_IRQHandler(void); // 70:TIMER5 
void TIMER6_IRQHandler(void); // 71:TIMER6
void DMA1_Channel0_IRQHandler(void); // 72:DMA1 Channel0
void DMA1_Channel1_IRQHandler(void); // 73:DMA1 Channel1
void DMA1_Channel2_IRQHandler(void); // 74:DMA1 Channel2
void DMA1_Channel3_IRQHandler(void); // 75:DMA1 Channel3
void DMA1_Channel4_IRQHandler(void); // 76:DMA1 Channel4
void USBFS_IRQHandler(void); // 83:USBFS

#endif /* __GD32E10_IT_H */
