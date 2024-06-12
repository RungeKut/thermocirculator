#include "temperature_measurement.h"
#include "main.h"
#include "adc.h"

uint32_t calibration;
volatile uint16_t senseTemp1;
volatile uint16_t senseTemp2;
volatile uint16_t senseTempRadiator;
volatile uint16_t senseTempMCU;
uint16_t senseAdcRef;
const uint32_t ch[5] = {ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_VREFINT};
volatile uint16_t sense[5];
uint8_t chSwitch = 0;

void TempMeasInit(void){
	//__HAL_RCC_ADC1_CLK_ENABLE();
	//__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_ADC_Start(&hadc);
	HAL_ADC_Stop(&hadc);
	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
	calibration = HAL_ADC_GetValue(&hadc);
//	if (chSwitch >= 4) chSwitch = 0; else chSwitch++;
//	ADC_ChannelConfTypeDef sConfig = {0};
//  sConfig.Channel = ch[chSwitch];
//	HAL_ADC_ConfigChannel(&hadc, &sConfig);
	ADC1->CHSELR  = ch[chSwitch];
	HAL_ADC_Start_IT(&hadc);
}

void TempMeasExecuter(void){
	
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1) //check if the interrupt comes from ACD1
	{
		//HAL_ADC_Stop_IT(hadc);
		sense[chSwitch] = HAL_ADC_GetValue(hadc);
		if (chSwitch >= 4) chSwitch = 0; else chSwitch++;
//		ADC_ChannelConfTypeDef sConfig = {0};
//		sConfig.Channel = ch[chSwitch];
//		if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
//		{
//			Error_Handler();
//	  }
		
		ADC1->CHSELR  = ch[chSwitch];
		HAL_ADC_Start_IT(hadc);
	}
}