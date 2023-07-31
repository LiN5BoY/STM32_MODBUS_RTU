#ifndef TIMER_H
#define TIMER_H

extern volatile uint32_t last_capture_time;
extern const uint32_t max_frame_time; // 设定的最大帧传输时间，单位为定时器计数值



void Timer_Init(uint16_t arr,uint16_t psc);


#endif	

