#include "stepper_motor.h"

/*
 * 步进电机驱动: 传入PWM频率与方向控制步进电机
 *
 * 参数:
 *   motor - 电机编号: STEPPER_LS / STEPPER_RS
 *   freq  - PWM频率(Hz), 即步进脉冲频率, 范围约 15Hz-50000Hz
 *   dir   - 方向: STEPPER_FORWARD / STEPPER_REVERSE
 *
 * 说明:
 *   LS和RS共用TIM3, 频率(ARR)为两路共享。调用本函数设置某一路的频率时,
 *   会同时影响另一路的脉冲频率。各路可独立控制方向(DIR)和启停(CCR)。
 *   TIM3基础时钟 = 72MHz / 72 = 1MHz, ARR = 1000000/freq - 1
 */
void Stepper_Control(uint8_t motor, uint32_t freq, uint8_t dir)
{
    uint16_t arr;
    uint16_t ccr;

    if (freq == 0)
    {
        Stepper_Stop(motor);
        return;
    }

    /* 计算 ARR (PSC=71, 时钟1MHz): ARR = 1MHz / freq - 1 */
    arr = (uint16_t)(1000000 / freq - 1);
    if (arr == 0) arr = 1;

    /* 50% 占空比 */
    ccr = (uint16_t)((arr + 1) / 2);

    /* 设置方向引脚 */
    if (motor == STEPPER_LS)
    {
        GPIO_WriteBit(LS_DIR_PORT, LS_DIR_PIN, (BitAction)dir);
    }
    else if (motor == STEPPER_RS)
    {
        GPIO_WriteBit(RS_DIR_PORT, RS_DIR_PIN, (BitAction)dir);
    }
    else
    {
        return;
    }

    /* 更新 TIM3 频率 (ARR 为两路共享) */
    TIM_SetAutoreload(STEPPER_TIM, arr);

    /* 设置对应通道 50% 占空比并输出脉冲 */
    if (motor == STEPPER_LS)
    {
        TIM_SetCompare1(STEPPER_TIM, ccr);
    }
    else
    {
        TIM_SetCompare2(STEPPER_TIM, ccr);
    }

    /* 确保定时器使能 */
    TIM_Cmd(STEPPER_TIM, ENABLE);
}

/*
 * 步进电机停止: 将对应通道占空比置0, 停止脉冲输出
 *
 * 参数:
 *   motor - 电机编号: STEPPER_LS / STEPPER_RS
 */
void Stepper_Stop(uint8_t motor)
{
    if (motor == STEPPER_LS)
    {
        TIM_SetCompare1(STEPPER_TIM, 0);
    }
    else if (motor == STEPPER_RS)
    {
        TIM_SetCompare2(STEPPER_TIM, 0);
    }
}
