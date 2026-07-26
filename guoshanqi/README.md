# ROSCAR 工程说明

基于 STM32F103C8T6 的多电机控制工程,通过 PWM 驱动 4 个直流电机与 2 个步进电机,并由 2 个轻触开关触发不同运动模式。

## 一、硬件配置

### 主控
- **MCU**: STM32F103C8T6 (Cortex-M3, 72MHz, 64KB Flash, 20KB SRAM)

### 外设引脚分配

#### 直流电机 (4个, PWM控制)
| 电机 | PWM引脚 | AIN1 | AIN2 | 定时器通道 |
|------|---------|------|------|-----------|
| L1   | PA0     | PB0  | PB1  | TIM2_CH1  |
| L2   | PA1     | PB10 | PB3  | TIM2_CH2  |
| R1   | PA2     | PB4  | PB5  | TIM2_CH3  |
| R2   | PA3     | PB6  | PB7  | TIM2_CH4  |

#### 步进电机 (2个, PWM+DIR控制)
| 电机 | PWM引脚 | DIR引脚 | 定时器通道 |
|------|---------|---------|-----------|
| LS   | PA6     | PA5     | TIM3_CH1  |
| RS   | PA7     | PB8     | TIM3_CH2  |

#### 按键 (2个, 轻触开关)
| 按键 | 引脚 | 触发方式 |
|------|------|---------|
| K1   | PB12 | 低电平有效(上拉输入) |
| K2   | PB13 | 低电平有效(上拉输入) |

### 机械结构
```
L1         R1

L2         R2

LS         RS
```

## 二、工程目录结构

```
郭山琦/
├── README.md                  # 本说明文件
├── 结构.txt                   # 原始结构要求
├── 要求.txt                   # 原始任务要求
└── project0/                  # Keil MDK工程
    ├── ROSCAR.uvprojx         # Keil工程文件
    ├── CORE/                  # Cortex-M3内核与启动文件
    │   ├── core_cm3.c
    │   ├── core_cm3.h
    │   └── startup_stm32f10x_md.s
    ├── STM32F10x_FWLib/       # STM32标准外设库
    │   ├── inc/
    │   └── src/
    ├── USER/                  # 用户应用层
    │   ├── main.c             # 主程序: 按键事件与运动控制
    │   ├── stm32f10x_it.c     # 中断服务程序
    │   ├── stm32f10x_it.h
    │   ├── stm32f10x.h
    │   ├── stm32f10x_conf.h   # 库配置文件
    │   ├── system_stm32f10x.c # 系统时钟配置
    │   └── system_stm32f10x.h
    ├── CODE/                  # 自编驱动代码
    │   ├── INC/               # 头文件
    │   │   ├── bsp.h          # BSP统一头文件
    │   │   ├── dc_motor.h     # 直流电机驱动声明
    │   │   ├── stepper_motor.h# 步进电机驱动声明
    │   │   └── key.h          # 按键驱动声明
    │   └── SRC/               # 源文件
    │       ├── bsp.c          # 硬件初始化 + 延时函数
    │       ├── dc_motor.c     # 直流电机驱动实现
    │       ├── stepper_motor.c# 步进电机驱动实现
    │       └── key.c          # 按键读取(含软件消抖)
    └── OBJ/                   # 编译输出
```

## 三、驱动函数说明

### 3.1 直流电机驱动

```c
void DC_Motor_Control(uint8_t motor, uint8_t duty, uint8_t dir);
```

| 参数 | 说明 |
|------|------|
| `motor` | `DC_MOTOR_L1` / `DC_MOTOR_L2` / `DC_MOTOR_R1` / `DC_MOTOR_R2` |
| `duty`  | 占空比 0-100 (%) |
| `dir`   | `DC_MOTOR_STOP`(停止) / `DC_MOTOR_FORWARD`(正转) / `DC_MOTOR_REVERSE`(反转) |

**PWM配置**: TIM2, 1kHz, 分辨率 0-999

**示例**:
```c
DC_Motor_Control(DC_MOTOR_L1, 50, DC_MOTOR_FORWARD);  // L1正转50%占空比
DC_Motor_Control(DC_MOTOR_R1, 80, DC_MOTOR_REVERSE);  // R1反转80%占空比
DC_Motor_Control(DC_MOTOR_L2, 0,  DC_MOTOR_STOP);     // L2停止
```

### 3.2 步进电机驱动

```c
void Stepper_Control(uint8_t motor, uint32_t freq, uint8_t dir);
void Stepper_Stop(uint8_t motor);
```

| 参数 | 说明 |
|------|------|
| `motor` | `STEPPER_LS` / `STEPPER_RS` |
| `freq`  | PWM脉冲频率(Hz), 即步进频率, 范围约 15Hz-50000Hz |
| `dir`   | `STEPPER_FORWARD`(正转) / `STEPPER_REVERSE`(反转) |

**PWM配置**: TIM3, 基础时钟 1MHz (PSC=71), ARR 按频率动态调整, 50%占空比

**圈数估算**: `步数 = 频率 × 时间`,例如 500Hz × 2s = 1000步,若电机为 200步/圈 则约 5 圈

**示例**:
```c
Stepper_Control(STEPPER_LS, 1000, STEPPER_FORWARD);  // LS正转1kHz
Stepper_Control(STEPPER_RS, 2000, STEPPER_REVERSE);  // RS反转2kHz
Stepper_Stop(STEPPER_LS);                            // LS停止
```

> **注意**: LS与RS共用TIM3,频率(ARR)为两路共享,各路可独立控制方向(DIR)和启停(CCR)。同时使用两路时频率需保持一致。

### 3.3 按键驱动

```c
uint8_t Key_Read(uint8_t key);
```

| 参数 | 说明 |
|------|------|
| `key` | `KEY_K1` / `KEY_K2` |
| 返回值 | `KEY_PRESSED`(1) / `KEY_RELEASED`(0) |

**消抖方式**: 检测到低电平后延时 10ms 再次确认

### 3.4 延时函数

```c
void Delay_ms(uint32_t ms);   // SysTick阻塞延时, 单位毫秒
```

## 四、运行逻辑

### 无按键
所有电机停止运行。

### K1 事件 (电平触发, 按住执行, 松开停止)
- L1、R1 同向正转,50% 转速

### K2 事件 (边沿触发, 按下一次执行完整序列)

| 阶段 | 动作 | 参考时间 |
|------|------|---------|
| ① 右侧正转 | R2正转(低速, 不满一周) + RS正转(约5圈) + L1保持转动 | 2000ms |
| ② 右侧回位 | RS反转(回原位) + R2反转(回原位), 结束后L1停止 | 2000ms |
| ③ 左侧正转 | L2正转(低速, 不满一周) + LS正转(约5圈) + R1保持转动 | 2000ms |
| ④ 左侧回位 | LS反转(回原位) + L2反转(回原位), 结束后全部停止 | 2000ms |

## 五、可调参数

`USER/main.c` 顶部宏定义:

```c
#define K2_DC_SHORT_DUTY   20    // R2/L2 不满一周的转速(%)
#define K2_STEPPER_FREQ    500   // 步进脉冲频率(Hz)
#define K2_STEPPER_TIME    2000  // 单步持续时间(ms), 约5圈
#define K2_HOLD_DUTY       50    // L1/R1 保持转动转速(%)
#define K1_DUTY            50    // K1 事件转速(%)
```

## 六、Keil 工程配置

### 添加 Include Path
`Options for Target` → `C/C++` → `Include Paths` 添加:
```
..\CODE\INC
```

### 添加源文件到工程分组
在 Keil 工程中新建分组(如 `CODE`),将以下文件加入:
```
CODE/SRC/bsp.c
CODE/SRC/dc_motor.c
CODE/SRC/stepper_motor.c
CODE/SRC/key.c
```

### 调试器配置
- **JTAG 引脚冲突**: 由于 PB3(JTDO)、PB4(JNTRST) 被用作电机方向控制,代码中已通过 `GPIO_Remap_SWJ_JTAGDisable` 禁用 JTAG,仅保留 SWD 调试。烧录时请选择 **SWD** 模式。

## 七、关键技术点

1. **JTAG 引脚复用**: 通过 `GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE)` 释放 PB3/PB4,保留 SWD 调试端口。
2. **TIM2 PWM**: 4 通道(PA0-PA3)同时输出,频率 1kHz,占空比独立可调。
3. **TIM3 频率可变 PWM**: PSC 固定为 71(1MHz基础时钟),运行时通过 `TIM_SetAutoreload()` 修改 ARR 实现频率调整,50% 占空比通过 CCR = ARR/2 计算。
4. **按键消抖**: SysTick 阻塞延时,两次读取确认。
5. **K2 边沿触发**: 通过 `k2_prev` 状态变量检测下降沿,长按不会重复触发,且等待按键释放后才执行序列。
