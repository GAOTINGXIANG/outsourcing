#include "key.h"
#include "bsp.h"

/*
 * 按键读取函数 (内部完成软件消抖)
 *
 * 参数:
 *   key - 按键编号: KEY_K1 / KEY_K2
 *
 * 返回值:
 *   KEY_PRESSED(1)  - 按键按下
 *   KEY_RELEASED(0) - 按键释放
 *
 * 说明:
 *   按键为上拉输入, 按下时引脚为低电平。
 *   消抖方式: 检测到低电平后延时10ms再次读取确认。
 */
uint8_t Key_Read(uint8_t key)
{
    GPIO_TypeDef *port;
    uint16_t      pin;

    if (key == KEY_K1)
    {
        port = K1_PORT;
        pin  = K1_PIN;
    }
    else if (key == KEY_K2)
    {
        port = K2_PORT;
        pin  = K2_PIN;
    }
    else
    {
        return KEY_RELEASED;
    }

    /* 检测按键是否按下(低电平) */
    if (GPIO_ReadInputDataBit(port, pin) == Bit_RESET)
    {
        Delay_ms(10);   /* 软件消抖延时 */
        if (GPIO_ReadInputDataBit(port, pin) == Bit_RESET)
        {
            return KEY_PRESSED;
        }
    }

    return KEY_RELEASED;
}
