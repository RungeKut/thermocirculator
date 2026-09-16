#ifndef TEMPERATURE_MEASUREMENT
#define TEMPERATURE_MEASUREMENT

#include "main.h"

extern volatile float currentTemp;
extern volatile float senseTemp1;
extern volatile float senseTemp2;
extern volatile float senseTempRadiator;
extern volatile float senseTempMCU;

/* BEGIN Prototypes */
void TempMeasInit(void);
void TempMeasExecuter(void);
/* END Prototypes */

#endif /* TEMPERATURE_MEASUREMENT */
