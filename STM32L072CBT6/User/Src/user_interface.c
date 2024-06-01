#include "user_interface.h"
#include "LCD1602.h"
#include "main.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define StrLength 8 //Длина видимой части строки на дисплее
#define Bounce 30 //задержка антидребезга, мс
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
		GoRunLine = len - StrLength;
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
	LCD_TextOnOff( 1 );
	for( uint8_t i = 0; i < 15; i++ ){
		LCD_StringRunL();
		HAL_Delay (TimeRunLine);
	}
}

//---ЭКРАНЫ МЕНЮ---
//Переключение по нажатию клавиши "MENU"
void ShowScreen(uint8_t s){
	GoRunLine = 0;
	LCD_Clear();
	switch (s){
		case 0:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("RUN");
			break;
		case 1:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("TEMP");
			LCD_SetCursor( 2, 1 );	
			LCD_PrintString("60°C");
			break;
		case 2:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("TIME");
			//LCD_SetCursor( 2, 1 );	
			LCD_PrintActiveString(TimeToStr(workTime), 1);
			break;
		case 3:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("PAUS");
			LCD_SetCursor( 2, 1 );	
			LCD_PrintString("60°C");
			break;
		case 4:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("STOP");
			LCD_SetCursor( 2, 1 );	
			LCD_PrintString("60°C");
			break;
		case 5:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("PROG 1");
			break;
		case 6:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("Pr1 TEMP");
			LCD_SetCursor( 2, 1 );	
			LCD_PrintString("60°C");
			break;
		case 7:
			LCD_SetCursor( 2, 0 );	
			LCD_PrintString("Pr1 TIME");
			LCD_SetCursor( 2, 1 );	
			LCD_PrintString("01:15");
			break;
	}
}

//---ОПРОС КНОПОК---
unsigned long RKTime = 0; //Переменная для таймера опроса кнопок

void ReadKey(void){
	if (HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port, START_BUTTON_Pin)){
		ShowScreen(0);
		workTime++;
	}
	if (HAL_GPIO_ReadPin(STOP_BUTTON_GPIO_Port, STOP_BUTTON_Pin)){
		ShowScreen(2);
	}
	if (HAL_GPIO_ReadPin(MENU_BUTTON_GPIO_Port, MENU_BUTTON_Pin)){
		ShowScreen(3);
		workTime--;
	}
	if (HAL_GPIO_ReadPin(LEFT_BUTTON_GPIO_Port, LEFT_BUTTON_Pin)){
		ShowScreen(4);
	}
	if (HAL_GPIO_ReadPin(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin)){
		ShowScreen(5);
	}
}

void UserInterfaceExecuter(void){
	ReadKey();
	RunLineExecuter();
}
