#ifndef __BSP_H
#define __BSP_H

#include "stm32f10x.h"

/* 包含各模块头文件 */
#include "dc_motor.h"
#include "stepper_motor.h"
#include "key.h"

/* 函数声明 */
void BSP_Init(void);
void Delay_ms(uint32_t ms);

#endif /* __BSP_H */
