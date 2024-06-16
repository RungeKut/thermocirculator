#include "user_interface.h"
#include "LCD1602.h"
#include "main.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "temperature_measurement.h"
#include "heater_control.h"

#define StrLength 8 //Длина видимой части строки на дисплее
#define TimeRunLine 250 //интервал бегущей строки, мс
#define ProgrammCount 5 //Количество программ

uint8_t GoRunLine = 0; //Флаг необходимости движения строк
uint8_t countRunLine = 0; //Счетчик движения строк
char RunLineLR = 'L'; //Флаг направления движения строк
uint32_t workTime = 3600; // max: 4294967295 Уставка времени работы
float workTemp = 60; //Уставка температуры
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
			bufPos += sprintf(bufPos, "%.0f", workTemp);
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
			LCD_PrintActiveString("PROG 1", 0);
			break;
		}
		case 1:{
			LCD_PrintActiveString("Pr1 TEMP", 0);
			LCD_PrintActiveString("60°C", 1);
			break;
		}
		case 2:{
			LCD_PrintActiveString("Pr1 TIME", 0);
			LCD_PrintActiveString(TimeToStr(workTime), 1);
			break;
		}
	}
}

//---ПРИВЕТСТВИЕ---
void Greeting(void){
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
		switch (keys){
			case 0x01:{ //Right
				switch (currentScreen){
					case 1:{ //TEMP
						if (speedKeys == 1){
							workTemp++;
						}
						else{
							workTemp += 5 * (speedKeys - 1);
						}
						ShowWorkScreen(currentScreen);
						break;
					}
					case 2:{ //TIME
						if (speedKeys == 1){
							workTime++;
						}
						else{
							workTime += 60 ^ (speedKeys - 1);
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
							workTemp--;
						}
						else{
							workTemp -= 5 * (speedKeys - 1);
						}
						ShowWorkScreen(currentScreen);
						break;
					}
					case 2:{ //TIME
						if (speedKeys == 1){
							workTime--;
						}
						else{
							workTime -= 60 ^ (speedKeys - 1);
						}
						ShowWorkScreen(currentScreen);
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
			if ( levels == 0 ){
				switch (stateCurrent){
					case 'R':{
						stateCurrent = 'P';
						currentScreen = 4;
						ShowWorkScreen(currentScreen);
						break;
					}
					case 'P':{
						stateCurrent = 'R';
						currentScreen = 3;
						ShowWorkScreen(currentScreen);
						break;
					}
					case 'S':{
						stateCurrent = 'R';
						currentScreen = 3;
						ShowWorkScreen(currentScreen);
						break;
					}
				}
			}
			break;
		}
		case 0x08:{ //Menu
			if ( stateCurrent == 'S'){ //Только в режиме работы - Stop
				if (speedKeys == 1){
					if ( currentScreen == 5 ){ //Чтобы из экрана Stop переходить в Time минуя Waiting
						currentScreen = 1;
					}
					else{
						currentScreen++;
						if ( currentScreen > 2 ) currentScreen = 0;
					}
					ShowWorkScreen(currentScreen);
				}
			}
			break;
		}
		case 0x10:{ //Stop
			if ( stateCurrent == 'R' || stateCurrent == 'P'){ //Только в режимах работы - Run и Pause
				stateCurrent = 'S';
				currentScreen = 5;
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
						stateCurrent = 'P';
						currentScreen = 4;
						ShowWorkScreen(currentScreen);
						break;
					}
					case 'P':{
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
						break;
					}
					case 'S':{
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

//Метод непрерывного обновления экрана в работе
void UpdateScreen(void){
	if ( currentScreen == 3 || currentScreen == 4 || currentScreen == 5 ){ //Только экраны Run Pause и Stop
		if ( GetTickDifference(USTime) > 3000 ){ // мс, время обновления
			ShowWorkScreen(currentScreen);
			USTime = HAL_GetTick();
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
}
