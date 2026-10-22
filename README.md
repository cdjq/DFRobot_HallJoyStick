# DFRobot_HallJoyStick

- [中文版](./README_CN.md)

Gravity Hall JoyStick reads raw X and Y counts and a Z button. The module speaks I2C or UART. The dial COM_SEL chooses one of those buses at reset. UART uses DFRobot General RTU 1.0 at 9600 8N1, and the register values are 16-bit big-endian.

## Product Link (https://www.dfrobot.com)

SKU: DFR1330

## Table of Contents

* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary

* Supports I2C address 0x33 or 0x34, selected by ADD_SEL at reset
* Supports UART slave address 0x33 at 9600 8N1
* Reads raw X and Y counts in the range 0..4095
* Reads the Z key as pressed or released

## Installation

1. Search `DFRobot_HallJoyStick` in the Arduino Library Manager and install it. Also install `DFRobot_RTU`.
2. Download this repository, place `DFRobot_HallJoyStick` in the Arduino `libraries` folder, then open an example from File > Examples.

## Methods

```C++
  /**
   * @fn begin
   * @brief Check that the module answers with the expected identity
   * @return true when PID is 0x1330 and VID is 0x3343
   * @n Returns false when either register does not match or the bus read fails.
   * @n UART uses DFRobot General RTU 1.0 at 9600 8N1. I2C uses a 1-byte register address and big-endian register values.
   */
  virtual bool begin(void) = 0;

  /**
   * @fn getJoystickXYData
   * @brief Read the raw X and Y counts
   * @param rawX X count, range 0..4095
   * @param rawY Y count, range 0..4095
   * @return 0 on success, 1 when the read fails
   * @n The values are raw ADC counts. This library does not convert them to voltage or angle.
   * @n One read uses input registers 0x0006, 0x0007 and 0x0008. The Z value from that read is kept for the next getJoystickZKeyState(), so a joystick poll does not start a second transfer.
   * @n A failed read leaves the caller buffers unchanged.
   */
  virtual int getJoystickXYData(uint16_t *rawX, uint16_t *rawY) = 0;

  /**
   * @fn getJoystickZKeyState
   * @brief Read the Z key
   * @return true when the key is pressed, false when it is released
   * @n true means pressed. The module key has an external pull-up and the pressed pin level is low; the firmware already converts that into this logic value.
   * @n When getJoystickXYData() has just succeeded, this returns the Z value from that same sample and does not read the bus again.
   * @n A failed read returns false.
   */
  virtual bool getJoystickZKeyState(void) = 0;

  /**
   * @fn DFRobot_HallJoyStick_I2C
   * @brief Bind an I2C bus and a 7-bit address
   * @param pWire I2C bus, default &Wire
   * @param i2cAddr 7-bit address, 0x33 when ADD_SEL is low, 0x34 when ADD_SEL is high
   * @n ADD_SEL is sampled by the module at reset. Passing the other address makes begin() return false.
   */
  DFRobot_HallJoyStick_I2C(TwoWire *pWire = &Wire, uint8_t i2cAddr = HALL_JOYSTICK_I2C_ADDRESS_LOW);

  /**
   * @fn DFRobot_HallJoyStick_UART
   * @brief Bind a hardware serial port
   * @param hSerial Hardware serial object
   * @param baud Baud rate, 9600
   * @param rxpin RX pin used by boards that remap Serial, otherwise ignored
   * @param txpin TX pin used by boards that remap Serial, otherwise ignored
   * @n The UART slave address is fixed at 0x33. ADD_SEL does not change it.
   * @n Frames are DFRobot General RTU 1.0. CRC stays inside DFRobot_RTU.
   */
  DFRobot_HallJoyStick_UART(HardwareSerial *hSerial, uint32_t baud = HALL_JOYSTICK_UART_BAUD, uint8_t rxpin = 0, uint8_t txpin = 0);
```

## Compatibility

| Board | Status | Note |
|-------|--------|------|
| Arduino Mega 2560 | I2C passed. UART reads, some frames fail | I2C on SDA 20 / SCL 21. UART on Serial1 RX 19 / TX 18 at 9600 |
| Arduino UNO | Untested | I2C on A4 / A5, UART on SoftwareSerial RX 10 / TX 11 |
| ESP32 | Untested | Pass the remapped RX and TX pins to the UART constructor |

## History

- 2026/10/22 - V1.0.0 version

## Credits

Written by wxzed(xiao.wu@dfrobot.com), 2026. (Welcome to our [website](https://www.dfrobot.com/))
