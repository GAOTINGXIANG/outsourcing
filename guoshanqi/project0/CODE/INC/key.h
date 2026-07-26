#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

/* 按键编号 */
#define KEY_K1   0
#define KEY_K2   1

/* 按键状态 */
#define KEY_RELEASED   0
#define KEY_PRESSED    1

/* K1: PB12 */
#define K1_PORT     GPIOB
#define K1_PIN      GPIO_Pin_12

/* K2: PB13 */
#define K2_PORT     GPIOB
#define K2_PIN      GPIO_Pin_13

/* 函数声明 */
uint8_t Key_Read(uint8_t key);

#endif /* __KEY_H */
