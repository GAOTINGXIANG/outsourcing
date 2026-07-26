#include "bsp.h"

/*
 * 硬件初始化: GPIO + TIM2(直流电机PWM) + TIM3(步进电机PWM)
 *
 * 引脚分配:
 *   直流电机 L1: PWM=PA0(TIM2_CH1), AIN1=PB0, AIN2=PB1
 *   直流电机 L2: PWM=PA1(TIM2_CH2), AIN1=PB10, AIN2=PB3
 *   直流电机 R1: PWM=PA2(TIM2_CH3), AIN1=PB4, AIN2=PB5
 *   直流电机 R2: PWM=PA3(TIM2_CH4), AIN1=PB6, AIN2=PB7
 *   步进电机 LS: PWM=PA6(TIM3_CH1), DIR=PA5
 *   步进电机 RS: PWM=PA7(TIM3_CH2), DIR=PB8
 *   按键     K1: PB12  K2: PB13
 */
void BSP_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef       TIM_OCInitStructure;

    /* 1. 开启时钟: GPIOA, GPIOB, AFIO(重映射用), TIM2, TIM3 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

    /* 2. 禁用JTAG, 释放PB3(JTDO)/PB4(JNTRST)为普通IO, 保留SWD调试 */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* 3. GPIO 初始化 */

    /* PA0-PA3: 直流电机 PWM (TIM2 CH1-CH4) - 复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA6, PA7: 步进电机 PWM (TIM3 CH1, CH2) - 复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA5: 步进电机LS方向 - 推挽输出 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PB0,PB1,PB3,PB4,PB5,PB6,PB7,PB8,PB10: 电机方向控制 - 推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3  |
                                  GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  |
                                  GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PB12, PB13: 按键 - 上拉输入(按下为低电平) */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 4. TIM2 初始化 (直流电机 PWM)
     * 72MHz / 72(PSC+1) / 1000(ARR+1) = 1kHz, 占空比范围 0-999 */
    TIM_TimeBaseStructure.TIM_Period        = DC_MOTOR_TIM_ARR;
    TIM_TimeBaseStructure.TIM_Prescaler     = DC_MOTOR_TIM_PSC;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* TIM2 PWM 配置 (CH1-CH4), 初始占空比为0 */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);

    /* 使能 CCR 预装载 */
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_Cmd(TIM2, ENABLE);

    /* 5. TIM3 初始化 (步进电机 PWM)
     * 72MHz / 72(PSC+1) = 1MHz 基础时钟, ARR 运行时按频率动态设置 */
    TIM_TimeBaseStructure.TIM_Period        = 999;
    TIM_TimeBaseStructure.TIM_Prescaler     = STEPPER_TIM_PSC;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* TIM3 PWM 配置 (CH1, CH2), 初始占空比为0(不输出脉冲) */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    /* 使能 ARR 预装载, 便于运行时修改频率 */
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_Cmd(TIM3, ENABLE);

    /* 6. 初始状态: 所有电机停止 */
    /* 直流电机 AIN1/AIN2 = 0 */
    GPIO_ResetBits(GPIOB, GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3  | GPIO_Pin_4 |
                          GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_10);
    /* 步进电机 DIR = 0 */
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

/*
 * SysTick 阻塞延时 (毫秒)
 * 72MHz 时钟, 重装载值 72000 对应 1ms
 */
void Delay_ms(uint32_t ms)
{
    SysTick->LOAD = 72000 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while (ms--)
    {
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
    SysTick->CTRL = 0;
    SysTick->VAL  = 0;
}
