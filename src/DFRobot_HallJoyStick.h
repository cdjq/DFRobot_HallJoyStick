/*!
 * @file DFRobot_HallJoyStick.h
 * @brief Define the DFRobot_HallJoyStick class
 * @details Header for the Gravity Hall JoyStick: I2C and UART subclasses, register addresses and public API.
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-22
 * @url https://github.com/DFRobot/DFRobot_HallJoyStick
 */
#ifndef __DFROBOT_HALLJOYSTICK_H__
#define __DFROBOT_HALLJOYSTICK_H__

#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_RTU.h>

#if defined(ESP8266) || defined(ARDUINO_AVR_UNO)
#include <SoftwareSerial.h>
#else
#include <HardwareSerial.h>
#endif

#define HALL_JOYSTICK_PID              0x1330  ///< product id, input register 0x0000
#define HALL_JOYSTICK_VID              0x3343  ///< vendor id, input register 0x0001
#define HALL_JOYSTICK_UART_ADDRESS     0x33    ///< UART slave address, fixed, not changed by ADD_SEL
#define HALL_JOYSTICK_I2C_ADDRESS_LOW  0x33    ///< I2C address when ADD_SEL is low
#define HALL_JOYSTICK_I2C_ADDRESS_HIGH 0x34    ///< I2C address when ADD_SEL is high
#define HALL_JOYSTICK_REG_PID          0x0000  ///< input register of the product id
#define HALL_JOYSTICK_REG_VID          0x0001  ///< input register of the vendor id
#define HALL_JOYSTICK_REG_X            0x0006  ///< input register of raw X
#define HALL_JOYSTICK_REG_Y            0x0007  ///< input register of raw Y
#define HALL_JOYSTICK_REG_Z            0x0008  ///< input register of the Z key
#define HALL_JOYSTICK_UART_BAUD        9600    ///< UART baud rate, 8N1
#define HALL_JOYSTICK_RAW_MIN          0       ///< lower bound of a raw axis count
#define HALL_JOYSTICK_RAW_MAX          4095    ///< upper bound of a raw axis count

/**
 * @enum eHallJoyStickAddress_t
 * @brief I2C address selected by ADD_SEL at power-up
 */
typedef enum {
  eHallAddrLow  = 0x33,  ///< ADD_SEL low
  eHallAddrHigh = 0x34   ///< ADD_SEL high
} eHallJoyStickAddress_t;

/**
 * @enum eHallJoyStickKey_t
 * @brief Z key value after the firmware converts the active-low pin
 */
typedef enum {
  eHallKeyReleased = 0,  ///< Z released
  eHallKeyPressed  = 1   ///< Z pressed
} eHallJoyStickKey_t;

class DFRobot_HallJoyStick {
public:
  /**
   * @fn DFRobot_HallJoyStick
   * @brief Base constructor
   * @n The module samples COM_SEL and ADD_SEL once after reset. This library does not change address, baud or calibration.
   */
  DFRobot_HallJoyStick();

  /**
   * @fn ~DFRobot_HallJoyStick
   * @brief Base destructor
   * @n Does not talk to the module.
   */
  virtual ~DFRobot_HallJoyStick();

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
};

class DFRobot_HallJoyStick_I2C : public DFRobot_HallJoyStick {
public:
  /**
   * @fn DFRobot_HallJoyStick_I2C
   * @brief Bind an I2C bus and a 7-bit address
   * @param pWire I2C bus, default &Wire
   * @param i2cAddr 7-bit address, 0x33 when ADD_SEL is low, 0x34 when ADD_SEL is high
   * @n ADD_SEL is sampled by the module at reset. Passing the other address makes begin() return false.
   */
  DFRobot_HallJoyStick_I2C(TwoWire *pWire = &Wire, uint8_t i2cAddr = HALL_JOYSTICK_I2C_ADDRESS_LOW);

  /**
   * @fn begin
   * @brief Start I2C and check PID/VID
   * @return true when PID is 0x1330 and VID is 0x3343
   * @n Returns false when either register does not match or the bus read fails.
   */
  virtual bool begin(void);

  /**
   * @fn getJoystickXYData
   * @brief Read the raw X and Y counts over I2C
   * @param rawX X count, range 0..4095
   * @param rawY Y count, range 0..4095
   * @return 0 on success, 1 when the read fails
   * @n The values are raw ADC counts. This library does not convert them to voltage or angle.
   * @n The same transfer also reads Z. The next getJoystickZKeyState() uses that value.
   */
  virtual int getJoystickXYData(uint16_t *rawX, uint16_t *rawY);

  /**
   * @fn getJoystickZKeyState
   * @brief Read the Z key over I2C
   * @return true when the key is pressed, false when it is released
   * @n true means pressed. The firmware already converts the active-low pin into this logic value.
   * @n When getJoystickXYData() has just succeeded, this returns Z from that same sample and does not read the bus again.
   * @n A failed read returns false.
   */
  virtual bool getJoystickZKeyState(void);

protected:
  bool readRegisters(uint8_t reg, uint16_t *data, uint8_t count);

private:
  TwoWire *_pWire;
  uint8_t _addr;
  uint16_t _cachedZ;
  bool _hasCachedZ;
};

class DFRobot_HallJoyStick_UART : public DFRobot_HallJoyStick, public DFRobot_RTU {
public:
#if defined(ESP8266) || defined(ARDUINO_AVR_UNO)
  /**
   * @fn DFRobot_HallJoyStick_UART
   * @brief Bind a software serial port
   * @param sSerial Software serial object. The sketch wiring RX/TX must match this object.
   * @param baud Baud rate, 9600
   * @n The UART slave address is fixed at 0x33. ADD_SEL does not change it.
   */
  DFRobot_HallJoyStick_UART(SoftwareSerial *sSerial, uint32_t baud = HALL_JOYSTICK_UART_BAUD);
#else
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
#endif

  /**
   * @fn begin
   * @brief Start UART and check PID/VID
   * @return true when PID is 0x1330 and VID is 0x3343
   * @n Returns false when either register does not match or the bus read fails.
   * @n The port is opened at 9600 8N1. This call does not change the module baud.
   */
  virtual bool begin(void);

  /**
   * @fn getJoystickXYData
   * @brief Read the raw X and Y counts over UART
   * @param rawX X count, range 0..4095
   * @param rawY Y count, range 0..4095
   * @return 0 on success, 1 when the read fails
   * @n The values are raw ADC counts. This library does not convert them to voltage or angle.
   * @n Function code 0x04 reads input 0x0006 for 3 registers. X and Y are returned, and Z is kept for the next getJoystickZKeyState().
   */
  virtual int getJoystickXYData(uint16_t *rawX, uint16_t *rawY);

  /**
   * @fn getJoystickZKeyState
   * @brief Read the Z key over UART
   * @return true when the key is pressed, false when it is released
   * @n true means pressed. The firmware already converts the active-low pin into this logic value.
   * @n When getJoystickXYData() has just succeeded, this returns Z from that same sample and does not read the bus again.
   * @n A failed read returns false.
   */
  virtual bool getJoystickZKeyState(void);

private:
  void openPort(void);

  Stream *_stream;
  uint32_t _baud;
  uint8_t _rxpin;
  uint8_t _txpin;
  uint16_t _cachedZ;
  bool _hasCachedZ;
#if defined(ESP8266) || defined(ARDUINO_AVR_UNO)
  SoftwareSerial *_port;
#else
  HardwareSerial *_port;
#endif
};

#endif
