#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Serial.h"
#include "Timer.h"

uint8_t time_val;
extern uint8_t Serial_TxPacket[100];                             // 发送内容




int main(void){

	Key_Init();
	Serial_Init();

	Timer_Init(72000 - 1,	4000 - 1);					// 设置重装载寄存器值
	OLED_Init();

	OLED_ShowString(1,1,"TxPacket");
	OLED_ShowString(3,1,"RxPacket"); 

    // 启用USART接收中断和定时器输入捕获中断
    configure_input_capture();


	while(1){
	


		if(Serial_GetRxFlag() == 1){
			
			OLED_ShowHexNum(4, 1, Serial_RxPacket[0], 2);
			OLED_ShowHexNum(4, 4, Serial_RxPacket[1], 2);
			OLED_ShowHexNum(4, 7, Serial_RxPacket[6], 2);
			OLED_ShowHexNum(4, 10, Serial_RxPacket[7], 2);
			
			OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);
			OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
			OLED_ShowHexNum(2, 7, Serial_TxPacket[23], 2);
			OLED_ShowHexNum(2, 10, Serial_TxPacket[24], 2);

			Data_Resolve();
			
		}



		// OLED_ShowNum(2,1,TIM_GetCounter(TIM3),10);

	}

	
}
   