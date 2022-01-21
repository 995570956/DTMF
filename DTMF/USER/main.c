#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "adc.h"
#include "goertzel.h"
#include "oled.h"
#include "exti.h"
#include "beep.h"
 
/************************************************
 ALIENTEK��ӢSTM32������ʵ��17
 ADC ʵ��   
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


 int main(void)
 {	 
//	 u16 i=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
  LED_Init();			     //LED�˿ڳ�ʼ��
  EXTIX_Init();         	//��ʼ���ⲿ�ж����� 
  OLED_Init();			//��ʼ��OLED  	 
 	TIM1_Init();
  ADC1_DMA1_Init();
  ADC1_Init();
  TIM_Cmd(TIM1, ENABLE);

     LED0=0;					//�ȵ������
	while(1)
	{
//		for(i=2;i<NPT/2;i++)
//		{
//			if(MagBufArray[i]>Max_Val)
//			{
//				Max_Val=MagBufArray[i];
//				Row=i;
//			}
//		}	
		//printf("\r\n��ӢSTM32������ ����ʵ��\r\n");
		//GetPowerMag();	
 //  delay_ms(1000);
		;
	}
 }

