#ifndef __DC_MOTOR_H
#define __DC_MOTOR_H

#include "stm32f10x.h"

/* 直流电机编号 */
#define DC_MOTOR_L1   0
#define DC_MOTOR_L2   1
#define DC_MOTOR_R1   2
#define DC_MOTOR_R2   3

/* 方向定义 */
#define DC_MOTOR_STOP       0   /* 停止 */
#define DC_MOTOR_FORWARD    1   /* 正转 */
#define DC_MOTOR_REVERSE    2   /* 反转 */

/* PWM定时器参数: TIM2, 72MHz / 72 / 1000 = 1kHz, 占空比分辨率0-999 */
#define DC_MOTOR_TIM            TIM2
#define DC_MOTOR_TIM_PSC        71
#define DC_MOTOR_TIM_ARR        999

/* L1: PWM=PA0(TIM2_CH1), AIN1=PB0, AIN2=PB1 */
#define L1_PWM_PORT         GPIOA
#define L1_PWM_PIN          GPIO_Pin_0
#define L1_AIN1_PORT        GPIOB
#define L1_AIN1_PIN         GPIO_Pin_0
#define L1_AIN2_PORT        GPIOB
#define L1_AIN2_PIN         GPIO_Pin_1

/* L2: PWM=PA1(TIM2_CH2), AIN1=PB10, AIN2=PB3 */
#define L2_PWM_PORT         GPIOA
#define L2_PWM_PIN          GPIO_Pin_1
#define L2_AIN1_PORT        GPIOB
#define L2_AIN1_PIN         GPIO_Pin_10
#define L2_AIN2_PORT        GPIOB
#define L2_AIN2_PIN         GPIO_Pin_3

/* R1: PWM=PA2(TIM2_CH3), AIN1=PB4, AIN2=PB5 */
#define R1_PWM_PORT         GPIOA
#define R1_PWM_PIN          GPIO_Pin_2
#define R1_AIN1_PORT        GPIOB
#define R1_AIN1_PIN         GPIO_Pin_4
#define R1_AIN2_PORT        GPIOB
#define R1_AIN2_PIN         GPIO_Pin_5

/* R2: PWM=PA3(TIM2_CH4), AIN1=PB6, AIN2=PB7 */
#define R2_PWM_PORT         GPIOA
#define R2_PWM_PIN          GPIO_Pin_3
#define R2_AIN1_PORT        GPIOB
#define R2_AIN1_PIN         GPIO_Pin_6
#define R2_AIN2_PORT        GPIOB
#define R2_AIN2_PIN         GPIO_Pin_7

/* 函数声明 */
void DC_Motor_Control(uint8_t motor, uint8_t duty, uint8_t dir);

#endif /* __DC_MOTOR_H */
