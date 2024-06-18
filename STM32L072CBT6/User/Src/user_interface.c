#include "user_interface.h"
#include "LCD1602.h"
#include "main.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "temperature_measurement.h"
#include "heater_control.h"
#include "tim.h"
#include "flash_work.h"

#define StrLength 8 //Длина видимой части строки на дисплее
#define TimeRunLine 250 //интервал бегущей строки, мс
#define ProgrammCount 16 //Количество программ

uint8_t GoRunLine = 0; //Флаг необходимости движения строк
uint8_t countRunLine = 0; //Счетчик движения строк
char RunLineLR = 'L'; //Флаг направления движения строк
uint32_t workTime = 3600; // max: 4294967295 Уставка времени работы
volatile float targetTemperature = 60; //Целевая температура
char finishBuf[30];  //Переменная для преобразования int в строку даты и времени
uint8_t currentScreen = 0; //Текущий отображаемый экран
char stateSL = 't'; //Что отображать во второй строке температуру(t) или время(T) попеременно
uint32_t currentTime = 0; //Текущее время
volatile uint8_t keys = 0; //Битовое состояние клавиатуры
volatile uint8_t speedKeys = 1; //Длительность нажатия клавиатуры (скорость изменения)
uint32_t BCTime; //Переменная для таймера действий кнопок
uint32_t RKTime; //Переменная для таймера опроса кнопок
char stateRK = 'S'; //Флаг запуска таймера антидребезка клавиатуры (Block) (Set)
uint32_t RLTime; //Переменная для таймера опроса уровня воды
volatile uint8_t levels = 0; //Битовое состояние датчиков уровня
char stateRL = 'S'; //Флаг запуска таймера антидребезка датчиков уровня (Block) (Set)
uint32_t USTime; //Переменная для таймера обновления экрана
volatile char stateCurrent = 'S'; //Текущее рабочее состояние R-run P-pause S-stop
volatile char stateRun = 'N'; //Текущее рабочее состояние N-нагрев S-стабилизация температуры
char stateSound = 'M'; //Текущее состояние воспроизведения звука M-тихо H-завершен нагрев
uint32_t inactivityTime; //Переменная для таймера бездействия оператора
uint8_t programScreen = 0; //Текущий отображаемый экран программы
uint8_t selectedProgram = 1; //Номер выбранной программы
volatile float progTemperature; //Температура выбранной программы
volatile uint32_t progTime; //Время выбранной программы
uint32_t SRTime; //Время повторения мелодии


//Автоскролинг строки
void LCD_PrintActiveString( char* str, uint8_t strNum ){
	uint8_t len = strlen (str);
  if (len <= StrLength){
		LCD_SetCursor( (StrLength - len) / 2, strNum );
		LCD_PrintString(str);
		GoRunLine = 0;
	}
	else{
		LCD_SetCursor( 0, strNum );
		LCD_PrintString(str);
		if (GoRunLine < len - StrLength) GoRunLine = len - StrLength;
		countRunLine = 0;
	}
}

//Исполнитель движения строк на дисплее
void RunLineExecuter(void){
	if (GoRunLine != 0){
		if (countRunLine == 0){
			if ( RunLineLR == 'R' ) RunLineLR = 'S';
			else RunLineLR = 'L';
		}
		else if (countRunLine == GoRunLine){
			if ( RunLineLR == 'L' ) RunLineLR = 'S';
			else RunLineLR = 'R';
		}
		if (RunLineLR == 'L'){
			LCD_StringRunL();
			HAL_Delay (TimeRunLine);
			countRunLine++;
		}
		else if (RunLineLR == 'R'){
			LCD_StringRunR();
			HAL_Delay (TimeRunLine);
			countRunLine--;
		}
		else if (RunLineLR == 'S'){
			HAL_Delay (3*TimeRunLine);
		}
	}
}

//Преобразование числа секунд в строку даты и времени
char* TimeToStr(uint32_t t){
	uint8_t sec = t % 60; //Секунды
	uint32_t temp = t / 60;
	uint8_t min = temp % 60; //Минуты
	temp = temp / 60;
	uint8_t hour = temp % 24; //Часы
	temp = temp / 24; //Дни
	char *finishBufPos = finishBuf;
	if (temp != 0){ //Если есть дни - то пишем сколько
		finishBufPos += sprintf(finishBufPos, "%d", temp);
		finishBufPos += sprintf(finishBufPos, ":");
	}
	if (hour != 0){ //Если есть часы - то пишем сколько
		if (hour < 10) finishBufPos += sprintf(finishBufPos, "0");
		finishBufPos += sprintf(finishBufPos, "%d", hour);
		finishBufPos += sprintf(finishBufPos, ":");
	}
	if (min < 10) finishBufPos += sprintf(finishBufPos, "0");
	finishBufPos += sprintf(finishBufPos, "%d", min);
	finishBufPos += sprintf(finishBufPos, ":");
	if (sec < 10) finishBufPos += sprintf(finishBufPos, "0");
	finishBufPos += sprintf(finishBufPos, "%d", sec);
	finishBufPos = finishBuf;
	return finishBufPos;
}

//---ЭКРАНЫ МЕНЮ---
void ShowWorkScreen(uint8_t s){
	GoRunLine = 0;
	LCD_Clear();
	switch (s){
		case 0:{
			LCD_PrintActiveString("WAITING", 0);
			break;
		}
		case 1:{
			LCD_PrintActiveString("TEMP", 0);
			char buf[8] = {0}, *bufPos = buf;
			bufPos += sprintf(bufPos, "%.0f", targetTemperature);
			bufPos += sprintf(bufPos, "°C");
			LCD_PrintActiveString(buf, 1);
			break;
		}
		case 2:{
			LCD_PrintActiveString("TIME", 0);
			LCD_PrintActiveString(TimeToStr(workTime), 1);
			break;
		}
		case 3:{
			LCD_PrintActiveString("RUN", 0);
			switch (stateSL){
				case 't':{
					char buf[8] = {0}, *bufPos = buf;
					bufPos += sprintf(bufPos, "%.1f", currentTemp);
					bufPos += sprintf(bufPos, "°C");
					LCD_PrintActiveString(buf, 1);
					stateSL = 'T';
					break;
				}
				case 'T':{
					LCD_PrintActiveString(TimeToStr(currentTime), 1);
					stateSL = 't';
					break;
				}
			}
			break;
		}
		case 4:
			LCD_PrintActiveString("PAUS", 0);
			switch (stateSL){
				case 't':{
					char buf[8] = {0}, *bufPos = buf;
					bufPos += sprintf(bufPos, "%.1f", currentTemp);
					bufPos += sprintf(bufPos, "°C");
					LCD_PrintActiveString(buf, 1);
					stateSL = 'T';
					break;
				}
				case 'T':
					LCD_PrintActiveString(TimeToStr(currentTime), 1);
					stateSL = 't';
					break;
			}
			break;
		case 5:{
			LCD_PrintActiveString("STOP", 0);
			char buf[8] = {0}, *bufPos = buf;
			bufPos += sprintf(bufPos, "%.1f", currentTemp);
			bufPos += sprintf(bufPos, "°C");
			LCD_PrintActiveString(buf, 1);
			break;
		}
		default:{
			break;
		}
	}
}

//Экраны меню программ
void ShowProgramScreen(uint8_t s){
	GoRunLine = 0;
	LCD_Clear();
	switch (s){
		case 0:{
			char buf[8] = {0}, *bufPos = buf;
			bufPos += sprintf(bufPos, "PROG ");
			bufPos += sprintf(bufPos, "%d", selectedProgram);
			LCD_PrintActiveString(buf, 0);
			break;
		}
		case 1:{
			char buf[8] = {0}, *bufPos = buf;
			bufPos += sprintf(bufPos, "Pr");
			bufPos += sprintf(bufPos, "%d", selectedProgram);
			bufPos += sprintf(bufPos, " TEMP");
			LCD_PrintActiveString(buf, 0);
			char buf1[8] = {0}, *bufPos1 = buf1;
			bufPos1 += sprintf(bufPos1, "%.0f", progTemperature);
			bufPos1 += sprintf(bufPos1, "°C");
			LCD_PrintActiveString(buf1, 1);
			break;
		}
		case 2:{
			char buf[8] = {0}, *bufPos = buf;
			bufPos += sprintf(bufPos, "Pr");
			bufPos += sprintf(bufPos, "%d", selectedProgram);
			bufPos += sprintf(bufPos, " TIME");
			LCD_PrintActiveString(buf, 0);
			LCD_PrintActiveString(TimeToStr(progTime), 1);
			break;
		}
	}
}

//---ПРИВЕТСТВИЕ---
void Greeting(void){
	flash_StartUp();
	LCD_SetCursor( 8, 0 );	
	LCD_PrintString("Привет!"); //max 38 символов
	LCD_SetCursor( 8, 1 );	
	LCD_PrintString("Версия ПО 0.1.0.263");
	LCD_TextOnOff( 1 );
	for( uint8_t i = 0; i < 30; i++ ){
		LCD_StringRunL();
		HAL_Delay (TimeRunLine);
	}
	ShowWorkScreen(currentScreen);
}

//---ОПРОС КНОПОК---
uint8_t GetKey(void){
	return ((GPIOA->IDR & 0x00F0) >> 3) + (GPIOB->IDR & 0x0001); //Start, Stop, Menu, Left, Right
}
//---ОПРОС КОНЦЕВИКОВ УРОВНЯ---
uint8_t GetLevel(void){
	return ((GPIOA->IDR & 0x0008) >> 2) + ((GPIOC->IDR & 0x2000) >> 13); //High level, Low level
}
//---Получение интервала таймера с момента t---
uint32_t GetTickDifference(uint32_t t){
	if ( HAL_GetTick() >= t) return HAL_GetTick() - t;
	else return 0xFFFFFFFF - t + HAL_GetTick();
}

//Метод для повторяющихся действий по кнопке
void ButtonClickExecuter(void){
	if ( GetTickDifference(BCTime) > 200 ){ // мс, частота действия по кнопке
		switch (stateCurrent){
			case 'S':{ //В режиме Stop
				switch (keys){
					case 0x01:{ //Right
						switch (currentScreen){
							case 1:{ //TEMP
								if (speedKeys == 1){
									targetTemperature++;
								}
								else{
									targetTemperature += speedKeys;
								}
								if ( targetTemperature > 100) targetTemperature = 20;
								ShowWorkScreen(currentScreen);
								break;
							}
							case 2:{ //TIME
								if (speedKeys == 1){
									workTime++;
								}
								else{
									workTime += round(pow(60, (speedKeys - 1)));
								}
								ShowWorkScreen(currentScreen);
								break;
							}
						}
						break;
					}
					case 0x04:{ //Left
						switch (currentScreen){
							case 1:{ //TEMP
								if (speedKeys == 1){
									targetTemperature--;
								}
								else{
									targetTemperature -= speedKeys;
								}
								if ( targetTemperature < 20) targetTemperature = 100;
								ShowWorkScreen(currentScreen);
								break;
							}
							case 2:{ //TIME
								if (speedKeys == 1){
									workTime--;
								}
								else{
									workTime -= round(pow(60, (speedKeys - 1)));
								}
								ShowWorkScreen(currentScreen);
								break;
							}
						}
						break;
					}
				}
				break;
			}
			case 'X':{ //В режиме Программирования
				switch (keys){
					case 0x01:{ //Right
						switch (programScreen){
							case 0:{ //PROG
								selectedProgram++;
								if ( selectedProgram > ProgrammCount) selectedProgram = 1;
								ShowProgramScreen(programScreen);
								break;
							}
							case 1:{ //TEMP
								if (speedKeys == 1){
									progTemperature++;
								}
								else{
									progTemperature += speedKeys;
								}
								if ( progTemperature > 100) progTemperature = 20;
								ShowProgramScreen(programScreen);
								break;
							}
							case 2:{ //TIME
								if (speedKeys == 1){
									progTime++;
								}
								else{
									progTime += round(pow(60, (speedKeys - 1)));
								}
								ShowProgramScreen(programScreen);
								break;
							}
						}
						break;
					}
					case 0x04:{ //Left
						switch (programScreen){
							case 0:{ //PROG
								selectedProgram--;
								if ( selectedProgram > ProgrammCount || selectedProgram == 0 ) selectedProgram = ProgrammCount;
								ShowProgramScreen(programScreen);
								break;
							}
							case 1:{ //TEMP
								if (speedKeys == 1){
									progTemperature--;
								}
								else{
									progTemperature -= speedKeys;
								}
								if ( progTemperature < 20) progTemperature = 100;
								ShowProgramScreen(programScreen);
								break;
							}
							case 2:{ //TIME
								if (speedKeys == 1){
									progTime--;
								}
								else{
									progTime -= round(pow(60, (speedKeys - 1)));
								}
								ShowProgramScreen(programScreen);
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
		BCTime = HAL_GetTick();
	}
}

//Метод для кнопок управления
void ButtonClickHandler(void){
	switch (keys){
		case 0x02:{ //Start
			if (speedKeys == 1){
				if ( levels == 0 && currentScreen != 1 && currentScreen != 2 && stateCurrent != 'X'){
					switch (stateCurrent){
						case 'R':{
							stateCurrent = 'P';
							currentScreen = 4;
							ShowWorkScreen(currentScreen);
							stateSound = 'M';
							break;
						}
						case 'P':{
							stateCurrent = 'R';
							currentScreen = 3;
							ShowWorkScreen(currentScreen);
							stateSound = 'M';
							break;
						}
						case 'S':{
							currentTime = workTime;
							stateCurrent = 'R';
							stateRun = 'N';
							currentScreen = 3;
							ShowWorkScreen(currentScreen);
							stateSound = 'M';
							break;
						}
					}
				}
				else if ( stateCurrent == 'X' ){
					if ( programScreen == 0 ){
						currentTime = progTime;
						targetTemperature = progTemperature;
						stateCurrent = 'R';
						stateRun = 'N';
						currentScreen = 3;
						ShowWorkScreen(currentScreen);
						stateSound = 'M';
					}
				}
			}
			break;
		}
		case 0x08:{ //Menu
			if ( stateCurrent == 'S'){ //Только в режиме работы - Stop
				if (speedKeys == 1){ //Короткое нажатие
					if ( currentScreen == 5 ){ //Чтобы из экрана Stop переходить в Time минуя Waiting
						currentScreen = 1;
					}
					else{
						currentScreen++;
						if ( currentScreen > 2 ){
							currentScreen = 0;
							stateSound = 'S';
						}
					}
					ShowWorkScreen(currentScreen);
				}
				else{ //Длительное нажатие
					stateCurrent = 'X';
					programScreen = 0;
					ShowProgramScreen(programScreen);
				}
			}
			else if ( stateCurrent == 'X'){ //Только в режиме программирования
				programScreen++;
				if ( programScreen > 2 ){
					programScreen = 0;
				}
				switch (programScreen){
					case 0:{ //PROG
						flashPage_write32data(1024, 2 * selectedProgram, progTime );
						stateSound = 'S';
						break;
					}
					case 1:{ //TEMP
						progTemperature = flashPage_read32(1024, 2 * selectedProgram - 1);
						break;
					}
					case 2:{ //TIME
						progTime = flashPage_read32(1024, 2 * selectedProgram);
						flashPage_write32data(1024, 2 * selectedProgram - 1, progTemperature);
						stateSound = 'S';
						break;
					}
				}
				ShowProgramScreen(programScreen);
			}
			break;
		}
		case 0x10:{ //Stop
			if ( stateCurrent == 'R' || stateCurrent == 'P'){ //Только в режимах работы - Run и Pause
				stateCurrent = 'S';
				currentScreen = 5;
				ShowWorkScreen(currentScreen);
			}
			else if ( stateCurrent == 'X' ){ //В режимах программирования
				stateCurrent = 'S';
				currentScreen = 0;
				ShowWorkScreen(currentScreen);
			}
			break;
		}
	}
}

//Метод проверки нажатия на клавиатуру
void ReadKey(void){
	if ( GetKey() != keys){
		if ( stateRK != 'B' ){
			RKTime = HAL_GetTick();
			stateRK = 'B';
		}
		if ( GetTickDifference(RKTime) > 50 ){ // мс, защита от дребезга
			keys = GetKey();
			speedKeys = 1;
			ButtonClickHandler();
			RKTime = HAL_GetTick();
		}
	}
	else if ( keys != 0 ){
		if (GetTickDifference(RKTime) > speedKeys * 2000 ){ // каждые T мс удержания увеличивай скорость на 1
			if ( speedKeys < 30 ){ // Предел "скорости" кнопки
				speedKeys++;
				ButtonClickHandler();
			}
		}
	}
	else{
		stateRK = 'S';
	}
}

//Метод проверки срабатывания датчиков уровня
void ReadLevel(void){
	if ( GetLevel() != levels){
		if ( stateRL != 'B' ){
			RLTime = HAL_GetTick();
			stateRL = 'B';
		}
		if ( GetTickDifference(RLTime) > 5000 ){ // мс, защита от дребезга и бултыхания
			levels = GetLevel();
			if ( levels != 0 ){ // Изменяем состояние индикатора "Капля"
				HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
				switch (stateCurrent){
					case 'R':{
						stateCurrent = 'S';
						currentScreen = 5;
						ShowWorkScreen(currentScreen);
						stateSound = 'A';
						break;
					}
					case 'P':{
						stateCurrent = 'S';
						currentScreen = 5;
						ShowWorkScreen(currentScreen);
						stateSound = 'A';
						break;
					}
					case 'S':{
						currentScreen = 0;
						ShowWorkScreen(currentScreen);
						break;
					}
				}
			}
			else{
				HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
				switch (stateCurrent){
					case 'R':{
						break;
					}
					case 'P':{
						stateCurrent = 'R';
						currentScreen = 3;
						ShowWorkScreen(currentScreen);
						stateSound = 'M';
						break;
					}
					case 'S':{
						stateSound = 'M';
						break;
					}
				}
			}
			RLTime = HAL_GetTick();
		}
	}
	else{
		stateRL = 'S';
	}
}

//Настройка таймера звука
void SetSoundFreq(float f){
	uint16_t reg = round(32000000/f);
	TIM2->ARR=reg;
	TIM2->CCR1=reg/2; 
}

//Плейер
void Sound(void){
	if ( GetTickDifference(SRTime) > 3000 ){ // мс, время повтора
		switch (stateSound){
			case 'M':{ //Тишина
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
				break;
			}
			case 'H':{ //Готовность
				SetSoundFreq(523.25); //Нота ДО
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
				HAL_Delay(200);
				SetSoundFreq(659.26); //Нота МИ
				HAL_Delay(200);
				SetSoundFreq(783.99); //Нота СОЛЬ
				HAL_Delay(200);
				SetSoundFreq(1046.50); //Нота ДО
				HAL_Delay(200);
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
				break;
			}
			case 'A':{ //Опасность
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
				for( uint8_t i = 0; i < 4; i++ ){
					SetSoundFreq(1864.66); //Нота ЛЯ#
					HAL_Delay(300);
					SetSoundFreq(1396.91); //Нота ФА
					HAL_Delay(300);
				}
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
				break;
			}
		}
		SRTime = HAL_GetTick();
	}
	// Без повторения
	switch (stateSound){
		case 'C':{ //Завершение
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			for( uint8_t i = 0; i < 30; i++ ){
				SetSoundFreq(523.25); //Нота ДО
				HAL_Delay(20);
				SetSoundFreq(659.26); //Нота МИ
				HAL_Delay(20);
				SetSoundFreq(783.99); //Нота СОЛЬ
				HAL_Delay(20);
				SetSoundFreq(1046.50); //Нота ДО
				HAL_Delay(20);
			}
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			stateSound = 'M';
			break;
		}
		case 'S':{ //Сохранение
			SetSoundFreq(1046.50); //Нота ДО
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			HAL_Delay(100);
			SetSoundFreq(783.99); //Нота СОЛЬ
			HAL_Delay(100);
			SetSoundFreq(659.26); //Нота МИ
			HAL_Delay(100);
			SetSoundFreq(523.25); //Нота ДО
			HAL_Delay(100);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			stateSound = 'M';
			break;
		}
	}
}

//Метод непрерывного обновления экрана в работе
void UpdateScreen(void){
	if ( currentScreen == 3 || currentScreen == 4 || currentScreen == 5 ){ //Только экраны Run Pause и Stop
		if ( GetTickDifference(USTime) > 3000 ){ // мс, время обновления
			USTime = HAL_GetTick(); //Для точности обновляем переменную вначале
			ShowWorkScreen(currentScreen);
			if (stateCurrent == 'P'){
				if ( GetTickDifference(inactivityTime) > 1200000 ){ // мс, время бездействия оператора 20 мин
					stateRun = 'N';
					stateCurrent = 'S';
					currentScreen = 5;
					ShowWorkScreen(currentScreen);
					stateSound = 'C';
				}
			}
			if ( stateRun == 'N' && stateCurrent == 'R' ){
				if ( targetTemperature - 5 <= currentTemp ){ //Недоходя 5°C звуковой сигнал о готовности
					stateRun = 'S';
					stateCurrent = 'P';
					currentScreen = 4;
					ShowWorkScreen(currentScreen);
					stateSound = 'H';
					inactivityTime = HAL_GetTick();
				}
			}
			if ( stateRun == 'S' && stateCurrent == 'R' ){
				if ( currentTime <= 3 ){ //Условие завершения программы
					stateRun = 'N';
					stateCurrent = 'S';
					currentScreen = 5;
					ShowWorkScreen(currentScreen);
					stateSound = 'C';
				}
				else{
					currentTime -= 3;
				}
			}
		}
	}
}

// Бесконечный цикл
void UserInterfaceExecuter(void){
	ReadKey();
	ReadLevel();
	ButtonClickExecuter();
	UpdateScreen();
	RunLineExecuter();
	TempMeasExecuter();
	HeaterExecuter();
	Sound();
}
