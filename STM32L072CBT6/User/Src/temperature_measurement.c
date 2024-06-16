#include "temperature_measurement.h"
#include "user_interface.h"
#include "main.h"
#include "adc.h"

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
const temperature_table_entry_type termo_table[] = {
    2275, 2183, 2093, 2003, 1915, 1828, 1742, 1659,
    1578, 1499, 1424, 1350, 1280, 1212, 1147, 1085,
    1026, 969, 915, 864, 816, 770, 727, 686,
    647, 611, 576, 544, 513, 485, 458, 432,
    408, 385, 364, 344, 325, 308, 291, 276,
    261
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