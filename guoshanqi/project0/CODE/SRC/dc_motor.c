#include "dc_motor.h"

/*
 * 直流电机驱动: 传入占空比与方向控制直流电机
 *
 * 参数:
 *   motor - 电机编号: DC_MOTOR_L1 / DC_MOTOR_L2 / DC_MOTOR_R1 / DC_MOTOR_R2
 *   duty  - 占空比 0-100 (百分比)
 *   dir   - 方向: DC_MOTOR_STOP / DC_MOTOR_FORWARD / DC_MOTOR_REVERSE
 *
 * 方向控制:
 *   正转: AIN1=1, AIN2=0
 *   反转: AIN1=0, AIN2=1
 *   停止: AIN1=0, AIN2=0, PWM=0
 */
void DC_Motor_Control(uint8_t motor, uint8_t duty, uint8_t dir)
{
    uint16_t ccr;
    GPIO_TypeDef *ain1_port, *ain2_port;
    uint16_t      ain1_pin,  ain2_pin;

    /* 占空比限幅 0-100, 转换为 CCR 值 (0-999) */
    if (duty > 100) duty = 100;
    ccr = (uint16_t)(duty * DC_MOTOR_TIM_ARR / 100);

    /* 根据电机编号选择引脚和PWM通道 */
    switch (motor)
    {
        case DC_MOTOR_L1:
            ain1_port = L1_AIN1_PORT; ain1_pin = L1_AIN1_PIN;
            ain2_port = L1_AIN2_PORT; ain2_pin = L1_AIN2_PIN;
            break;
        case DC_MOTOR_L2:
            ain1_port = L2_AIN1_PORT; ain1_pin = L2_AIN1_PIN;
            ain2_port = L2_AIN2_PORT; ain2_pin = L2_AIN2_PIN;
            break;
        case DC_MOTOR_R1:
            ain1_port = R1_AIN1_PORT; ain1_pin = R1_AIN1_PIN;
            ain2_port = R1_AIN2_PORT; ain2_pin = R1_AIN2_PIN;
            break;
        case DC_MOTOR_R2:
            ain1_port = R2_AIN1_PORT; ain1_pin = R2_AIN1_PIN;
            ain2_port = R2_AIN2_PORT; ain2_pin = R2_AIN2_PIN;
            break;
        default:
            return;
    }

    /* 设置方向引脚 */
    switch (dir)
    {
        case DC_MOTOR_FORWARD:
            GPIO_SetBits(ain1_port, ain1_pin);
            GPIO_ResetBits(ain2_port, ain2_pin);
            break;
        case DC_MOTOR_REVERSE:
            GPIO_ResetBits(ain1_port, ain1_pin);
            GPIO_SetBits(ain2_port, ain2_pin);
            break;
        case DC_MOTOR_STOP:
        default:
            GPIO_ResetBits(ain1_port, ain1_pin);
            GPIO_ResetBits(ain2_port, ain2_pin);
            ccr = 0;    /* 停止时 PWM 输出 0 */
            break;
    }

    /* 设置 PWM 占空比 */
    switch (motor)
    {
        case DC_MOTOR_L1: TIM_SetCompare1(DC_MOTOR_TIM, ccr); break;
        case DC_MOTOR_L2: TIM_SetCompare2(DC_MOTOR_TIM, ccr); break;
        case DC_MOTOR_R1: TIM_SetCompare3(DC_MOTOR_TIM, ccr); break;
        case DC_MOTOR_R2: TIM_SetCompare4(DC_MOTOR_TIM, ccr); break;
    }
}
