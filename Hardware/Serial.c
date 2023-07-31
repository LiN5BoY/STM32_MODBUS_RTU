#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include "Timer.h"
uint8_t Serial_RxPacket[100] = {0};
uint16_t Serial_RxLength = 0;
uint8_t Serial_RxFlag;
uint8_t clearBufferFlag = 0;

void Serial_Init(void){

    // 1.开启时钟(USART与GPIO)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);// USART
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);// GPIO
    // 2.GPIO初始化（TX——复用输出，RX——输入）
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
    // 3.配置USART
    USART_InitTypeDef USART_InitStructure = {
        .USART_BaudRate = 9600,// 波特率
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None, // 硬件流控制
        .USART_Mode = USART_Mode_Tx | USART_Mode_Rx,// 指定发送功能 如果又要发送也要接收 可以采用 A | B 的格式
        .USART_Parity = USART_Parity_No,// 校验位
        .USART_StopBits = USART_StopBits_1,// 停止位
        .USART_WordLength = USART_WordLength_8b
    };
    USART_Init(USART1,&USART_InitStructure);

    // 开启中断
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    // 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // 初始化NVIC的USART1通道
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelCmd = ENABLE,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1
    };
    NVIC_Init(&NVIC_InitStructure);

    // 4.开启USART(或配置中断)
    USART_Cmd(USART1,ENABLE);
}


void Serial_SendByte(uint8_t byte){
    USART_SendData(USART1,byte);
    // 等待TXE置为1
    // TXE 用于指示发送数据寄存器（Transmit Data Register）是否为空
    // ，即是否可以向USART发送新的数据。
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
    // 标志位是否需要手动清除？
    // 通过手册可知 不需要手动清0
}

void Serial_SendArray(uint8_t *Array,uint16_t Length){
    uint16_t i;
    for (i = 0 ; i < Length ; i++){
        Serial_SendByte(Array[i]);
    }
}

void Serial_SendString(char *String){
    uint8_t i;
    for(int i = 0 ; String[i] != '\0' ; i++){
        Serial_SendByte(String[i]);
    }
}

uint32_t Serial_Pow(uint32_t x,uint32_t y){
    uint32_t Result = 1;
    while (y--)
        Result *= x;
    return Result;
}



void Serial_SendNumber(uint32_t Number,uint8_t Length){
    uint8_t i;
    for(i = 0 ; i < Length ; i++){
        Serial_SendByte(Number / Serial_Pow(10,Length - i - 1) % 10 + '0');
    }
}

//进行printf的重定向设置
int fputc(int ch,FILE *f){
    Serial_SendByte(ch);
    return ch;
}

void Serial_Printf(char *format,...){
    char String[100];
    va_list arg;
    va_start(arg,format);
    vsprintf(String,format,arg);
    va_end(arg);
    Serial_SendString(String);
}



uint8_t Serial_GetRxFlag(void){
    if (Serial_RxFlag == 1){
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}



void USART1_IRQHandler(void){

	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
        // 接收到一个字节数据，记录定时器的当前值
        last_capture_time = TIM_GetCapture3(TIM3);
        Serial_RxPacket[Serial_RxLength++] = USART_ReceiveData(USART1);
        Serial_RxFlag = 1;
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);// 清除标志位
        
	}

}