#include "stm32f10x.h"


void LED_Init(void)
{
	//配备时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_2);
}

void LED_Turn(GPIO_TypeDef * g,uint16_t a){
	if(GPIO_ReadOutputDataBit(g,a)==0){
		GPIO_SetBits(g,a);
	}else{
		GPIO_ResetBits(g,a);
	}
}


void Set_LED(GPIO_TypeDef * g,uint16_t a,int x){
	if(x==0){
		//低电平
		GPIO_ResetBits(g,a);
	}else{
		//高电平
		GPIO_SetBits(g,a);
	}
}

