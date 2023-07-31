#ifndef __LED_H
#define __LED_H

void LED_Init(void);
void Set_LED(GPIO_TypeDef * g,uint16_t a,int x);
void LED_Turn(GPIO_TypeDef * g,uint16_t a);
#endif