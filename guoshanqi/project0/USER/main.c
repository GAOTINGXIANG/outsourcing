#include "stm32f10x.h"
#include "bsp.h"

/* ========== K2 事件运动序列参数(参考值, 可自行调整) ========== */

/* 直流电机 R2/L2 不满一周的转速(占空比%)与时间(ms) */
#define K2_DC_SHORT_DUTY     20      /* 低速, 确保不满一周 */
#define K2_DC_SHORT_TIME     1500    /* 短行程时间(ms) */

/* 步进电机 RS/LS 大约5圈的脉冲频率(Hz)与时间(ms)
 * 例: 500Hz * 2000ms = 1000脉冲, 若200步/圈则为5圈 */
#define K2_STEPPER_FREQ      500     /* 步进脉冲频率 Hz */
#define K2_STEPPER_TIME      2000    /* 转动时间(ms), 约5圈 */

/* 保持转动电机(L1/R1)的转速(占空比%) */
#define K2_HOLD_DUTY         50      /* 50% 转速 */

/* K1 事件参数 */
#define K1_DUTY              50      /* 50% 转速 */

/*
 * 停止所有电机
 */
static void All_Motors_Stop(void)
{
    DC_Motor_Control(DC_MOTOR_L1, 0, DC_MOTOR_STOP);
    DC_Motor_Control(DC_MOTOR_L2, 0, DC_MOTOR_STOP);
    DC_Motor_Control(DC_MOTOR_R1, 0, DC_MOTOR_STOP);
    DC_Motor_Control(DC_MOTOR_R2, 0, DC_MOTOR_STOP);
    Stepper_Stop(STEPPER_LS);
    Stepper_Stop(STEPPER_RS);
}

/*
 * K2 事件: 执行一段运动序列
 *
 * 第一阶段 (右侧):
 *   R2 正转 (不满一周)
 *   RS 正转 (约5圈)
 *   L1 保持转动
 *
 * 第二阶段 (右侧回位):
 *   RS 反转 (回到原位)
 *   R2 反转 (回到原位)
 *   L1 停止
 *
 * 第三阶段 (左侧):
 *   L2 正转 (不满一周)
 *   LS 正转 (约5圈)
 *   R1 保持转动
 *
 * 第四阶段 (左侧回位):
 *   LS 反转 (回到原位)
 *   L2 反转 (回到原位)
 *   R1 停止
 */
static void K2_Sequence(void)
{
    /* ---- 第一阶段: 右侧正转 ---- */
    DC_Motor_Control(DC_MOTOR_R2, K2_DC_SHORT_DUTY, DC_MOTOR_FORWARD);
    Stepper_Control(STEPPER_RS, K2_STEPPER_FREQ, STEPPER_FORWARD);
    DC_Motor_Control(DC_MOTOR_L1, K2_HOLD_DUTY, DC_MOTOR_FORWARD);
    Delay_ms(K2_STEPPER_TIME);

    /* ---- 第二阶段: 右侧回位 ---- */
    Stepper_Control(STEPPER_RS, K2_STEPPER_FREQ, STEPPER_REVERSE);
    DC_Motor_Control(DC_MOTOR_R2, K2_DC_SHORT_DUTY, DC_MOTOR_REVERSE);
    Delay_ms(K2_STEPPER_TIME);

    DC_Motor_Control(DC_MOTOR_L1, 0, DC_MOTOR_STOP);
    DC_Motor_Control(DC_MOTOR_R2, 0, DC_MOTOR_STOP);
    Stepper_Stop(STEPPER_RS);

    /* ---- 第三阶段: 左侧正转 ---- */
    DC_Motor_Control(DC_MOTOR_L2, K2_DC_SHORT_DUTY, DC_MOTOR_FORWARD);
    Stepper_Control(STEPPER_LS, K2_STEPPER_FREQ, STEPPER_FORWARD);
    DC_Motor_Control(DC_MOTOR_R1, K2_HOLD_DUTY, DC_MOTOR_FORWARD);
    Delay_ms(K2_STEPPER_TIME);

    /* ---- 第四阶段: 左侧回位 ---- */
    Stepper_Control(STEPPER_LS, K2_STEPPER_FREQ, STEPPER_REVERSE);
    DC_Motor_Control(DC_MOTOR_L2, K2_DC_SHORT_DUTY, DC_MOTOR_REVERSE);
    Delay_ms(K2_STEPPER_TIME);

    /* 全部停止 */
    All_Motors_Stop();
}

int main(void)
{
    uint8_t k2_prev = KEY_RELEASED;

    /* 硬件初始化: GPIO + TIM2(直流电机PWM) + TIM3(步进电机PWM) */
    BSP_Init();

    while (1)
    {
        /* K1: 按住执行, 松开停止 — L1 R1 同向转动(50%转速) */
        if (Key_Read(KEY_K1) == KEY_PRESSED)
        {
            DC_Motor_Control(DC_MOTOR_L1, K1_DUTY, DC_MOTOR_FORWARD);
            DC_Motor_Control(DC_MOTOR_R1, K1_DUTY, DC_MOTOR_FORWARD);
        }
        else
        {
            DC_Motor_Control(DC_MOTOR_L1, 0, DC_MOTOR_STOP);
            DC_Motor_Control(DC_MOTOR_R1, 0, DC_MOTOR_STOP);
        }

        /* K2: 边沿触发 — 检测到按下后执行完整运动序列(单次) */
        if ((Key_Read(KEY_K2) == KEY_PRESSED) && (k2_prev == KEY_RELEASED))
        {
            /* 等待按键释放, 避免长按重复触发 */
            while (Key_Read(KEY_K2) == KEY_PRESSED);

            /* 执行 K2 运动序列 */
            K2_Sequence();

            /* 序列执行完后重置状态, 等待下一次按下 */
            k2_prev = KEY_RELEASED;
        }
        else
        {
            k2_prev = Key_Read(KEY_K2);
        }
    }
}
