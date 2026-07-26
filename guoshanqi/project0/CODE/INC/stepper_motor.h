#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#include "stm32f10x.h"

/* 步进电机编号 */
#define STEPPER_LS   0
#define STEPPER_RS   1

/* 方向定义 */
#define STEPPER_FORWARD   0   /* 正转 */
#define STEPPER_REVERSE   1   /* 反转 */

/* PWM定时器参数: TIM3, 基础时钟1MHz(PSC=71), 频率通过ARR动态调整 */
#define STEPPER_TIM         TIM3
#define STEPPER_TIM_PSC     71

/* LS: PWM=PA6(TIM3_CH1), DIR=PA5 */
#define LS_PWM_PORT     GPIOA
#define LS_PWM_PIN      GPIO_Pin_6
#define LS_DIR_PORT     GPIOA
#define LS_DIR_PIN      GPIO_Pin_5

/* RS: PWM=PA7(TIM3_CH2), DIR=PB8 */
#define RS_PWM_PORT     GPIOA
#define RS_PWM_PIN      GPIO_Pin_7
#define RS_DIR_PORT     GPIOB
#define RS_DIR_PIN      GPIO_Pin_8

/* 函数声明 */
void Stepper_Control(uint8_t motor, uint32_t freq, uint8_t dir);
void Stepper_Stop(uint8_t motor);

#endif /* __STEPPER_MOTOR_H */
