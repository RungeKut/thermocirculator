#include "heater_control.h"
#include "main.h"
#include "temperature_measurement.h"
#include "user_interface.h"

#define PID_DUTY_CYCLE_MIN	0
#define PID_DUTY_CYCLE_MAX	1000

uint32_t HETime; //Переменная для таймера обработчика нагревателя
volatile float pwmDutyCycle = 500; //мс длительность импульса ШИМ-сигнала
float Kp = 100; //Пропорциональный коэффициент ПИД-регулятора
float Ki = 1; //Интегральный коэффициент ПИД-регулятора
float Kd = 500; //Дифференцирующий коэффициент ПИД-регулятора
volatile float errorPrevious = 0; //предыдущее значение невязки температуры
volatile float errorCurrent = 0; //текущее значение невязки температуры
volatile float errorIntegral = 0; //значение интегрирующей составляющей
volatile float errorDifferential = 0; //значение диффиренцирующей составляющей
uint32_t PWMTime; //Переменная для таймера PWM генератора

void PWMExecuter(void){
	if ( GetTickDifference(PWMTime) > PID_DUTY_CYCLE_MAX ){
		if ( pwmDutyCycle <= PID_DUTY_CYCLE_MIN) return;
		HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_SET); //Включение нагревателя
		PWMTime = HAL_GetTick();
	}
	else{
		if ( GetTickDifference(PWMTime) > pwmDutyCycle ){
			HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_RESET); //Отключение нагревателя
		}
	}
}

void PIDExecuter(void){
	errorCurrent = currentTemp - targetTemperature;
	//if ((((Ki * errorIntegral) <= PID_DUTY_CYCLE_MAX) && (errorCurrent >= 0)) || (((Ki * errorIntegral) >= PID_DUTY_CYCLE_MIN) && (errorCurrent < 0)))
  //{
    errorIntegral += errorCurrent;
  //}
	errorDifferential = errorCurrent - errorPrevious;
	pwmDutyCycle = - Kp * errorCurrent - Ki * errorIntegral - Kd * errorDifferential;
	if (pwmDutyCycle < PID_DUTY_CYCLE_MIN)
  {
     pwmDutyCycle = PID_DUTY_CYCLE_MIN;
  }
  if (pwmDutyCycle > PID_DUTY_CYCLE_MAX)
  {
    pwmDutyCycle = PID_DUTY_CYCLE_MAX;
  }
  errorPrevious = errorCurrent;
}

//Обработчик нагревателя
void HeaterExecuter(void){
	PWMExecuter();
	if ( GetTickDifference(HETime) > 1000 ){ // мс, время обновления (не стоит делать быстрее чем появляется новое значение измеренной температуры)
		switch (stateCurrent){
			case 'R':{
				HAL_GPIO_WritePin(DC_MOTOR_GPIO_Port, DC_MOTOR_Pin, GPIO_PIN_SET); //Включение мотора постоянного тока
				HAL_GPIO_WritePin(AC_MOTOR_GPIO_Port, AC_MOTOR_Pin, GPIO_PIN_SET); //Включение мотора переменного тока
				PIDExecuter();
				break;
			}
			case 'P':{
				switch (stateRun){
					case 'N':{
						errorPrevious = 0;
						errorCurrent = 0;
						errorIntegral = 0;
						errorDifferential = 0;
						pwmDutyCycle = 0;
						break;
					}
					case 'S':{
						PIDExecuter();
						break;
					}
				}
				break;
			}
			case 'S':{
				HAL_GPIO_WritePin(DC_MOTOR_GPIO_Port, DC_MOTOR_Pin, GPIO_PIN_RESET); //Отключение мотора постоянного тока
				HAL_GPIO_WritePin(AC_MOTOR_GPIO_Port, AC_MOTOR_Pin, GPIO_PIN_RESET); //Отключение мотора переменного тока
				errorPrevious = 0;
				errorCurrent = 0;
				errorIntegral = 0;
				errorDifferential = 0;
				pwmDutyCycle = 0;
				break;
			}
		}
		HETime = HAL_GetTick();
	}
}
