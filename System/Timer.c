#include "stm32f10x.h"                  // Device header
#include "Serial.h"
extern uint8_t time_val;

volatile uint32_t last_capture_time = 0;
const uint32_t max_frame_time = 4000; // 设定的最大帧传输时间，单位为定时器计数值

void Timer_Init(uint16_t arr,uint16_t psc){

	//RCC内部时钟 ON
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//时钟源选择
	TIM_InternalClockConfig(TIM3);
	
	//配置时机单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;						// 不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;					// 向上计数
	TIM_TimeBaseInitStructure.TIM_Period = arr ;									// 因为预分频器和计数器都有1个数的偏差，所以这里要再减去一个1
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc ;									// Tout = （（arr+1）*（psc+1））/Tclk ;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	
	
	TIM_ClearFlag(TIM3,TIM_IT_Update);
	//配置输出中断控制
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	
	//配置NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);									// 优先级分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
NVIC_InitStructure.NVIC_IRQChannel = TIM_Channel_3;									// 中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									// 制特定中断通道的使能状态
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;						// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								// 响应优先级
	NVIC_Init(&NVIC_InitStructure); 
	


	//启动定时器
	TIM_Cmd(TIM3,ENABLE);
}

// 配置定时器通道为输入捕获模式
void configure_input_capture() {
    // 配置输入捕获通道 CHx 为输入捕获模式
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0; // 将CC1S位设置为01，选择输入捕获通道1为TI1
    // 配置输入捕获通道 CHx 的触发边沿或状态变化条件
    TIM3->CCER |= TIM_CCER_CC1P; // 设置捕获边沿为下降沿触发，如果需要上升沿触发，可以选择设置为TIM_CCER_CC1NP
    // 使能捕获通道 CHx
    TIM3->CCER |= TIM_CCER_CC1E;
}


void TIM3_IRQHandler(void){

	if(TIM_GetITStatus(TIM3,TIM_IT_CC3) != RESET){	 // 输入捕获中断触发，计算两个捕获之间的时间间隔
		uint32_t current_capture_time = TIM_GetCapture1(TIM3);
        uint32_t time_interval = current_capture_time - last_capture_time;

		if (time_interval > max_frame_time) {
            // 超过设定的最大帧传输时间，认为一帧数据传输结束
            // 处理完整的数据帧
			Serial_RxFlag = 1;
        }



        // 重置定时器捕获时间
        last_capture_time = current_capture_time;

		TIM_ClearITPendingBit(TIM3,TIM_IT_CC3);
		

	}	

}




