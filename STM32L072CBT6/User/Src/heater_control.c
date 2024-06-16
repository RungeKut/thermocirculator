#include "heater_control.h"
#include "main.h"
#include "temperature_measurement.h"
#include "user_interface.h"

uint32_t HETime; //Переменная для таймера обработчика нагревателя

//Обработчик нагревателя
void HeaterExecuter(void){
	if ( GetTickDifference(HETime) > 1000 ){ // мс, время обновления
		switch (stateCurrent){
			case 'R':{
				HAL_GPIO_WritePin(DC_MOTOR_GPIO_Port, DC_MOTOR_Pin, GPIO_PIN_SET); //Включение мотора постоянного тока
				HAL_GPIO_WritePin(AC_MOTOR_GPIO_Port, AC_MOTOR_Pin, GPIO_PIN_SET); //Включение мотора переменного тока
				break;
			}
			case 'P':{
				
				break;
			}
			case 'S':{
				HAL_GPIO_WritePin(DC_MOTOR_GPIO_Port, DC_MOTOR_Pin, GPIO_PIN_RESET); //Отключение мотора постоянного тока
				HAL_GPIO_WritePin(AC_MOTOR_GPIO_Port, AC_MOTOR_Pin, GPIO_PIN_RESET); //Отключение мотора переменного тока
				break;
			}
		}
	}
}
