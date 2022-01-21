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
 ALIENTEK精英STM32开发板实验17
 ADC 实验   
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


 int main(void)
 {	 
//	 u16 i=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
  LED_Init();			     //LED端口初始化
  EXTIX_Init();         	//初始化外部中断输入 
  OLED_Init();			//初始化OLED  	 
 	TIM1_Init();
  ADC1_DMA1_Init();
  ADC1_Init();
  TIM_Cmd(TIM1, ENABLE);

     LED0=0;					//先点亮红灯
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
		//printf("\r\n精英STM32开发板 串口实验\r\n");
		//GetPowerMag();	
 //  delay_ms(1000);
		;
	}
 }

