#include "temperature_measurement.h"
#include "user_interface.h"
#include "main.h"
#include "adc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define V25 1.41 // В Напряжение при температуре 25ºC
#define Avg_Slope 4.3 // мВ/ºC Средняя чувствительность датчика температуры контроллера

uint16_t calibration;
volatile float currentTemp;
volatile float senseTemp1;
volatile float senseTemp2;
volatile float senseTempRadiator;
volatile float senseTempMCU;
volatile float senseAdcRef;
const uint32_t ch[5] = {ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_VREFINT};
volatile uint16_t sense[5];
uint8_t chSwitch = 0;
uint8_t chReady = 0;

void TempMeasInit(void){
	HAL_ADC_Start(&hadc);
	HAL_ADC_Stop(&hadc);
	HAL_Delay(10);
	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
	HAL_ADC_PollForConversion(&hadc, 1000);
	calibration = HAL_ADC_GetValue(&hadc);
}

// Значение температуры, возвращаемое если сумма результатов АЦП больше первого значения таблицы
#define TEMPERATURE_UNDER 200
// Значение температуры, возвращаемое если сумма результатов АЦП меньше последнего значения таблицы
#define TEMPERATURE_OVER 1000
// Значение температуры соответствующее первому значению таблицы
#define TEMPERATURE_TABLE_START 200
// Шаг таблицы 
#define TEMPERATURE_TABLE_STEP 20

// Тип каждого элемента в таблице, если сумма выходит в пределах 16 бит - uint16_t, иначе - uint32_t
typedef uint16_t temperature_table_entry_type;
// Тип индекса таблицы. Если в таблице больше 256 элементов, то uint16_t, иначе - uint8_t
typedef uint8_t temperature_table_index_type;

/* Таблица суммарного значения АЦП в зависимости от температуры. От большего значения к меньшему
   Для построения таблицы использованы следующие парамертры:
     R1(T1): 10кОм(25°С)
     Таблица R/T характеристик: EPCOS R/T:8016; B25/100:3988K
     Схема включения: A
     Ra: 10кОм
     Напряжения U0/Uref: 3.3В/3.3В
*/
/*
const temperature_table_entry_type termo_table[] = { //Парабола
2181,2097,2015,1936,1858,1782,1708,1636,
1566,1498,1432,1367,1305,1245,1187,1131,
1076,1024,974,926,879,835,792,752,
713,677,642,610,579,550,524,499,
476,456,437,420,405,392,381,373,366
};*/
const temperature_table_entry_type termo_table[] = { //Экспонента
2223,2122,2027,1936,1848,1765,1685,1610,1537,1468,1402,1338,1278,1221,1166,1113,1063,1015,
969,926,884,844,806,770,735,702,670,640,611,584,557,532,508,485,463,442,422,403,385,368,351
};

// Функция вычисляет значение температуры в десятых долях градусов Цельсия
// в зависимости от суммарного значения АЦП.
int16_t calc_temperature(temperature_table_entry_type adcsum) {
  temperature_table_index_type l = 0;
  temperature_table_index_type r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
  temperature_table_entry_type thigh = termo_table[r];
  
  // Проверка выхода за пределы и граничных значений
  if (adcsum <= thigh) {
    #ifdef TEMPERATURE_UNDER
      if (adcsum < thigh) 
        return TEMPERATURE_UNDER;
    #endif
    return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type tlow = termo_table[0];
  if (adcsum >= tlow) {
    #ifdef TEMPERATURE_OVER
      if (adcsum > tlow)
        return TEMPERATURE_OVER;
    #endif
    return TEMPERATURE_TABLE_START;
  }

  // Двоичный поиск по таблице
  while ((r - l) > 1) {
    temperature_table_index_type m = (l + r) >> 1;
    temperature_table_entry_type mid = termo_table[m];
    if (adcsum > mid) {
      r = m;
    } else {
      l = m;
    }
  }
  temperature_table_entry_type vl = termo_table[l];
  if (adcsum >= vl) {
    return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type vr = termo_table[r];
  temperature_table_entry_type vd = vl - vr;
  int16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP; 
  if (vd) {
    // Линейная интерполяция
    res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
  }
  return res;
}

uint32_t RTTime; //Переменная для таймера опроса датчиков температуры
void TempMeasExecuter(void){
	if ( chReady ){
		switch (chReady){
			case 1:
				senseTemp2 = calc_temperature(sense[0]) * senseAdcRef/12; //1.2 * sense[0]/sense[4];
				if ( senseTemp2 > senseTemp1) currentTemp = senseTemp2; else currentTemp = senseTemp1; //берем наибольшую температуру
				break;
			case 2:
				senseTemp1 = calc_temperature(sense[1]) * senseAdcRef/12; //1.2 * sense[1]/sense[4];
				break;
			case 3:
				senseTempRadiator = calc_temperature(sense[2]) * senseAdcRef/12; //1.2 * sense[2]/sense[4];
				break;
			case 4:
				senseTempMCU = ((V25-(1.2 * sense[3]/sense[4]))/Avg_Slope)+25;
				break;
			case 5:
				senseAdcRef = ((float)(sense[4])/4095)*3.3;
				break;
		}
		chReady = 0;
	}
	if ( GetTickDifference(RTTime) > 200 ){ // мс, 5 датчиков, каждый 1 раз в секунду.
		RTTime = HAL_GetTick();
		ADC1->CHSELR  = ch[chSwitch];
		HAL_Delay(10);
		HAL_ADC_Start_IT(&hadc);
		char buf[30] = {0}, *bufPos = buf;
		if ( sense[0] > 4080 || sense[1] > 4080 || sense[2] > 4080){
			bufPos += sprintf(bufPos, "Оbрыв датчика");
			if ( sense[0] > 4080 ){
				bufPos += sprintf(bufPos, " X7");
			}
			if ( sense[1] > 4080 ){
				bufPos += sprintf(bufPos, " X8");
			}
			if ( sense[2] > 4080 ){
				bufPos += sprintf(bufPos, " радиатора");
			}
			LCD_PrintActiveString(buf, 0);
			stateCurrent = 'A';
		}
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1) //check if the interrupt comes from ACD1
	{
		//HAL_ADC_Stop_IT(hadc);
		sense[chSwitch] = HAL_ADC_GetValue(hadc);
		chReady = chSwitch + 1;
		if (chSwitch >= 4) chSwitch = 0; else chSwitch++;
//		ADC_ChannelConfTypeDef sConfig = {0};
//		sConfig.Channel = ch[chSwitch];
//		if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
//		{
//			Error_Handler();
//	  }
		
//		ADC1->CHSELR  = ch[chSwitch];
//		HAL_ADC_Start_IT(hadc);
	}
}
