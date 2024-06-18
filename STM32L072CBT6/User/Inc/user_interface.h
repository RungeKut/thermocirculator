#ifndef USER_INTERFACE
#define USER_INTERFACE

#include "main.h"

/* BEGIN Prototypes */
void Greeting(void);
void UserInterfaceExecuter(void);
uint32_t GetTickDifference(uint32_t t);
extern volatile char stateCurrent;
extern volatile float targetTemperature;
extern volatile char stateRun;
void LCD_PrintActiveString( char* str, uint8_t strNum );
/* END Prototypes */

#endif /* USER_INTERFACE */
