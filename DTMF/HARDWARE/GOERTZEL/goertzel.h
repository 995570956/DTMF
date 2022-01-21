#ifndef __GOERTZEL_H
#define __GOERTZEL_H

#include "sys.h"
#include "adc.h"

#define NPT 205
//#define NPT 4480
extern int signalnum;
extern u8 display_position;
extern u8 num_total;
extern float InBufArray[NPT];
void GetPowerMag(void);
//void GetPowerMag(float arr[]);

#endif
