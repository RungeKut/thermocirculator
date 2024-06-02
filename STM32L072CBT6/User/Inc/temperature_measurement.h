#ifndef TEMPERATURE_MEASUREMENT
#define TEMPERATURE_MEASUREMENT

#include "main.h"

extern volatile uint16_t senseTemp1;
extern volatile uint16_t senseTemp2;
extern volatile uint16_t senseTempRadiator;
extern volatile uint16_t senseTempMCU;

/* BEGIN Prototypes */
void TempMeasInit(void);
void TempMeasExecuter(void);
/* END Prototypes */

#endif /* TEMPERATURE_MEASUREMENT */
