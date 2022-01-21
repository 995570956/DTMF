#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"


#define ARR 125-1
#define PSC 72-1
#define ADC_LEN    1
#define STOREPOINT    20

void ADC_Configuration(void);

void ADC1_Init(void);
void ADC1_DMA1_Init(void);
void TIM1_Init(void);
float array_maxs_value(float arr[]);
void grim_point_test(void);

#endif

