# DFRobot_HallJoyStick

- [English Version](./README.md)

Gravity 霍尔摇杆读取 X、Y 原始计数值和 Z 键。模块使用 I2C 或 UART，拨码 COM_SEL 在复位时选定其中一种。UART 为 DFRobot General RTU 1.0，9600 8N1，寄存器值是 16 位大端。

## 产品链接 (https://www.dfrobot.com)

SKU：DFR1330

## 目录

* [概述](#概述)
* [库安装](#库安装)
* [方法](#方法)
* [兼容性](#兼容性)
* [历史](#历史)
* [创作者](#创作者)

## 概述

* 支持 I2C 地址 0x33 或 0x34，由复位时的 ADD_SEL 决定
* 支持 UART 从站地址 0x33，9600 8N1
* 读取 X、Y 原始计数值，范围 0..4095
* 读取 Z 键按下或松开

## 库安装

1. 在 Arduino 库管理器中搜索 `DFRobot_HallJoyStick` 并安装，同时安装 `DFRobot_RTU`。
2. 下载本仓库，把 `DFRobot_HallJoyStick` 放到 Arduino 的 `libraries` 目录，再从文件 > 示例中打开示例。

## 方法

```C++
  /**
   * @fn begin
   * @brief 核对模块身份寄存器
   * @return PID 为 0x1330 且 VID 为 0x3343 时返回 true
   * @n 任一寄存器不匹配，或总线读取失败，返回 false。
   * @n UART 使用 DFRobot General RTU 1.0，9600 8N1。I2C 先写 1 字节寄存器地址，寄存器值为大端。
   */
  virtual bool begin(void) = 0;

  /**
   * @fn getJoystickXYData
   * @brief 读取 X 与 Y 的原始计数值
   * @param rawX X 计数值，范围 0..4095
   * @param rawY Y 计数值，范围 0..4095
   * @return 成功返回 0，读取失败返回 1
   * @n 返回值是原始 ADC 计数值。库不做电压或角度换算。
   * @n 一次读取输入寄存器 0x0006、0x0007 和 0x0008。这次读到的 Z 留给下一次 getJoystickZKeyState()，摇杆轮询不会再发第二帧。
   * @n 读取失败时不改调用方缓冲区。
   */
  virtual int getJoystickXYData(uint16_t *rawX, uint16_t *rawY) = 0;

  /**
   * @fn getJoystickZKeyState
   * @brief 读取 Z 键
   * @return 按下返回 true，松开返回 false
   * @n true 表示按下。
   * @n 若刚刚 getJoystickXYData() 成功，这里返回同一次采样的 Z，不再读总线。
   * @n 读取失败时返回 false。
   */
  virtual bool getJoystickZKeyState(void) = 0;

  /**
   * @fn DFRobot_HallJoyStick_I2C
   * @brief 绑定 I2C 总线和 7 位地址
   * @param pWire I2C 总线，默认 &Wire
   * @param i2cAddr 7 位地址。ADD_SEL 为低时是 0x33，为高时是 0x34
   * @n ADD_SEL 由模块在复位时采样。地址传错时 begin() 返回 false。
   */
  DFRobot_HallJoyStick_I2C(TwoWire *pWire = &Wire, uint8_t i2cAddr = HALL_JOYSTICK_I2C_ADDRESS_LOW);

  /**
   * @fn DFRobot_HallJoyStick_UART
   * @brief 绑定硬件串口
   * @param hSerial 硬件串口对象
   * @param baud 波特率，9600
   * @param rxpin 需要重映射串口的板子使用的 RX，其它板忽略
   * @param txpin 需要重映射串口的板子使用的 TX，其它板忽略
   * @n UART 从站地址固定为 0x33，ADD_SEL 不改变它。
   * @n 帧格式是 DFRobot General RTU 1.0。CRC 留在 DFRobot_RTU 内。
   */
  DFRobot_HallJoyStick_UART(HardwareSerial *hSerial, uint32_t baud = HALL_JOYSTICK_UART_BAUD, uint8_t rxpin = 0, uint8_t txpin = 0);
```

## 兼容性

主控               |  正常运行    |   运行失败    |   未测试    | 备注
------------------ | :----------: | :----------: | :---------: | :---:
Arduino Uno        |              |              |      √      |
Arduino MEGA2560   |      √       |              |             |
Arduino Leonardo   |              |              |      √      |
FireBeetle-ESP8266 |              |              |      √      |
FireBeetle-ESP32   |              |              |      √      |
FireBeetle-M0      |              |              |      √      |
Micro:bit          |              |              |      √      |

## 历史

- 2026/10/22 - V1.0.0 版本

## 创作者

Written by wxzed(xiao.wu@dfrobot.com), 2026. (欢迎访问我们的[网站](https://www.dfrobot.com/))
