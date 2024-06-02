#include "user_interface.h"
#include "LCD1602.h"
#include "main.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "temperature_measurement.h"

#define StrLength 8 //Длина видимой части строки на дисплее
#define TimeRunLine 250 //интервал бегущей строки, мс

//Автоскролинг строки
uint8_t GoRunLine = 0;
uint8_t countRunLine = 0;
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

char RunLineLR = 'L';
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

uint32_t workTime = 0; // max: 4294967295
char finishBuf[30];
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
}

//---ЭКРАНЫ МЕНЮ---
//Переключение по нажатию клавиши "MENU"
uint8_t currentScreen = 255;
void ShowScreen(uint8_t s){
	if (s == currentScreen) return;
	GoRunLine = 0;
	LCD_Clear();
	switch (s){
		case 0:
			LCD_PrintActiveString("RUN", 0);
			break;
		case 1:
			LCD_PrintActiveString("TEMP", 0);
			LCD_PrintActiveString("60°C", 1);
			break;
		case 2:
			LCD_PrintActiveString("TIME", 0);
			LCD_PrintActiveString(TimeToStr(workTime), 1);
			break;
		case 3:
			LCD_PrintActiveString("PAUS", 0);
			LCD_PrintActiveString("60°C", 1);
			break;
		case 4:
			LCD_PrintActiveString("STOP", 0);
			LCD_PrintActiveString("60°C", 1);
			break;
		case 5:
			LCD_PrintActiveString("PROG 1", 0);
			break;
		case 6:
			LCD_PrintActiveString("Pr1 TEMP", 0);
			LCD_PrintActiveString("60°C", 1);
			break;
		case 7:
			LCD_PrintActiveString("Pr1 TIME", 0);
			LCD_PrintActiveString(TimeToStr(workTime), 1);
			break;
	}
}

//---ОПРОС КНОПОК---
uint16_t GetKey(void){
	return ((GPIOA->IDR & 0x00F0) >> 3) + (GPIOB->IDR & 0x0001); //Start, Stop, Menu, Left, Right
}
uint32_t GetTickDifference(uint32_t t){
	if ( HAL_GetTick() >= t) return HAL_GetTick() - t;
	else return 0xFFFFFFFF - t + HAL_GetTick();
}
uint32_t RKTime; //Переменная для таймера опроса кнопок
uint16_t keys = 0;
uint8_t speedKeys = 1;
char stateRK = 'S';
void ReadKey(void){
	if ( GetKey() != keys){
		if ( stateRK != 'B' ){
			RKTime = HAL_GetTick();
			stateRK = 'B';
		}
		if ( GetTickDifference(RKTime) > 50 ){ // мс, защита от дребезга
			keys = GetKey();
			RKTime = HAL_GetTick();
			speedKeys = 1;
		}
	}
	else if ( keys != 0 ){
		if (GetTickDifference(RKTime) > speedKeys * 2000 ){ // каждые T мс удержания увеличивай скорость на 1
			if ( speedKeys < 20 ) speedKeys++;
		}
	}
	else{
		stateRK = 'S';
	}
}

//------
void UserInterfaceExecuter(void){
	ReadKey();
	RunLineExecuter();
	TempMeasExecuter();
}
