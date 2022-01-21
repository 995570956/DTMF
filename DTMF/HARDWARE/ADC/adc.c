#include "adc.h"
#include "delay.h"
#include "usart.h"	
#include "goertzel.h"
#include "math.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

uint16_t ADC_Value[ADC_LEN];
float point_test[STOREPOINT];
static u8 begin_test;
static u8 end_test=1;

void ADC1_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PC1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);//复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);//AD校准
    while(ADC_GetCalibrationStatus(ADC1));  
}

void ADC1_DMA1_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Value;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC | DMA_IT_HT, ENABLE);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void TIM1_Init(void)
{
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = ARR;          
    TIM_TimeBaseStructure.TIM_Prescaler = PSC;       
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
    TIM_OCInitStructure.TIM_Pulse = 25; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, DISABLE);
}

void DMA1_Channel1_IRQHandler(void)
{
	u16 static i=0;
	u16 static gezeer_point;
	u16 static start;//已经进入可检测范围内
	u16 static end;//已经出来可检测范围内
	u8 static iscompter;//在两个端点内是否已经检测过数字
	u16 temp_adc_value;

	
  DMA_Cmd(DMA1_Channel1, DISABLE);

  DMA_ClearITPendingBit(DMA_IT_HT);
  DMA_ClearITPendingBit(DMA1_IT_TC1);
	
	temp_adc_value=(signed short)(ADC_Value[0]);//要考虑的长度
	
	
	
	if(i<20){
	   point_test[i] = (float)temp_adc_value*(3.3f/4096.0f);
		 i++;
	}else{
	   point_test[19] = (float)temp_adc_value*(3.3f/4096.0f);
	}
	if(i == 20){
    //i=0;
		grim_point_test();
		if(begin_test==1 && end_test==0) start = 1;
		if(begin_test==1 && end_test==1) end = 1;
	}
	
	if(start == 1){
		if(iscompter == 0){
		  InBufArray[gezeer_point] = (float)temp_adc_value*(3.3/4096);
			//printf("%f\r\n",InBufArray[gezeer_point]);
		  gezeer_point++;
		}
	
		//printf("11111111111111");
	}
	if(end == 1){
	   gezeer_point =0;
		 start  = 0;
		 end = 0;
		iscompter = 0;
		//printf("222222222222222\r\n");
	}
  
	if(gezeer_point == 205){
	    GetPowerMag();
      gezeer_point = 0;
		  iscompter = 1;
	}
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void grim_point_test(void){
	  int m ;
	  float sum=0.0f;
    for(m=0;m<20;m++){
		//printf("test value is %f\r\n",point_test[m]);
		  sum += fabs(point_test[m]);
			if(m<19) point_test[m] = point_test[m+1];
			
		}
		//printf("========================\r\n");
		sum=sum/20.0;
		//printf("test value is %f\r\n",sum);
		if(sum > 0.25 && end_test == 1) {begin_test=1;end_test=0; }
		if(sum < 0.25 && begin_test == 1) { end_test=1; 
		       
		}
}

//void DMA1_Channel1_IRQHandler2(void)
//{
//	u16 static i = 0;
//	u16 temp;
//	u16 v;
//	u16 n;
//	u16 static badvalue;
//	u16 temp_adc_value;
//	float energy[NPT];
//	float maxvalue;
//	
//	DMA_Cmd(DMA1_Channel1, DISABLE);

//  DMA_ClearITPendingBit(DMA_IT_HT);
//  DMA_ClearITPendingBit(DMA1_IT_TC1);
//	
//	temp_adc_value=(signed short)(ADC_Value[0]);
//	InBufArray[i] = (float)temp_adc_value*(3.3/4096);
//  i++;
//	
//	if(i==205){
//		TIM_Cmd(TIM1, DISABLE);
//	  for(v=0;v<(205-20);v++){
//			 energy[v] = 0;
//		   for(n=v;n<v+20;n++){
//			    energy[v]+=InBufArray[n];
//			 }
//		}
//		maxvalue = array_maxs_value(energy);
//		for(v=0;v<205;v++){
//		   if(energy[v]/(2*maxvalue)<0.25){
//			    break;
//			 }
//			 
//			 if(v==185){ printf("success\r\n");GetPowerMag(); }
//		}
//		i=0; 
//		TIM_Cmd(TIM1, ENABLE);
//	}
//	DMA_Cmd(DMA1_Channel1, ENABLE);

//}


float array_maxs_value(float arr[]){
	 int i;
   float temp_value=arr[0];
	 for(i=1;i<205;i++){
	    if(arr[i] > temp_value){
			    temp_value = arr[i];
			}
	 }
	 return temp_value;
}





