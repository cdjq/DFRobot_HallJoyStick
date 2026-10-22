/*!
 * @file DFRobot_HallJoyStick.cpp
 * @brief Gravity Hall JoyStick host driver
 * @details I2C uses a 1-byte register address and big-endian values. UART uses DFRobot_RTU for CRC and function codes.
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-22
 * @url https://github.com/DFRobot/DFRobot_HallJoyStick
 */

#include "DFRobot_HallJoyStick.h"

DFRobot_HallJoyStick::DFRobot_HallJoyStick()
{
}

DFRobot_HallJoyStick::~DFRobot_HallJoyStick()
{
}

DFRobot_HallJoyStick_I2C::DFRobot_HallJoyStick_I2C(TwoWire *pWire, uint8_t i2cAddr)
  : _pWire(pWire), _addr(i2cAddr), _cachedZ(0), _hasCachedZ(false)
{
}

bool DFRobot_HallJoyStick_I2C::readRegisters(uint8_t reg, uint16_t *data, uint8_t count)
{
  uint8_t need;
  uint8_t i;

  if (_pWire == NULL || data == NULL || count == 0) {
    return false;
  }
  need = count * 2;
  _pWire->beginTransmission(_addr);
  _pWire->write(reg);
  if (_pWire->endTransmission(false) != 0) {
    return false;
  }
  if (_pWire->requestFrom((int)_addr, (int)need) != need) {
    return false;
  }
  for (i = 0; i < count; i++) {
    uint8_t hi = (uint8_t)_pWire->read();
    uint8_t lo = (uint8_t)_pWire->read();
    data[i] = ((uint16_t)hi << 8) | lo;
  }
  return true;
}

bool DFRobot_HallJoyStick_I2C::begin(void)
{
  uint16_t id[2] = {0, 0};

  if (_pWire == NULL) {
    return false;
  }
  _pWire->begin();
  _pWire->setClock(100000);
  if (!readRegisters(HALL_JOYSTICK_REG_PID, id, 2)) {
    return false;
  }
  return id[0] == HALL_JOYSTICK_PID && id[1] == HALL_JOYSTICK_VID;
}

int DFRobot_HallJoyStick_I2C::getJoystickXYData(uint16_t *rawX, uint16_t *rawY)
{
  uint16_t raw[3] = {0, 0, 0};

  _hasCachedZ = false;
  if (rawX == NULL || rawY == NULL) {
    return 1;
  }
  if (!readRegisters(HALL_JOYSTICK_REG_X, raw, 3)) {
    return 1;
  }
  *rawX = raw[0];
  *rawY = raw[1];
  _cachedZ = raw[2];
  _hasCachedZ = true;
  return 0;
}

bool DFRobot_HallJoyStick_I2C::getJoystickZKeyState(void)
{
  uint16_t z = 0;

  if (_hasCachedZ) {
    _hasCachedZ = false;
    return _cachedZ == eHallKeyPressed;
  }
  if (!readRegisters(HALL_JOYSTICK_REG_Z, &z, 1)) {
    return false;
  }
  return z == eHallKeyPressed;
}

#if defined(ESP8266) || defined(ARDUINO_AVR_UNO)
DFRobot_HallJoyStick_UART::DFRobot_HallJoyStick_UART(SoftwareSerial *sSerial, uint32_t baud)
  : DFRobot_RTU(sSerial), _stream(sSerial), _baud(baud), _rxpin(0), _txpin(0), _cachedZ(0), _hasCachedZ(false), _port(sSerial)
{
}
#else
DFRobot_HallJoyStick_UART::DFRobot_HallJoyStick_UART(HardwareSerial *hSerial, uint32_t baud, uint8_t rxpin, uint8_t txpin)
  : DFRobot_RTU(hSerial), _stream(hSerial), _baud(baud), _rxpin(rxpin), _txpin(txpin), _cachedZ(0), _hasCachedZ(false), _port(hSerial)
{
}
#endif

void DFRobot_HallJoyStick_UART::openPort(void)
{
  if (_port == NULL) {
    return;
  }
#if defined(ESP32)
  _port->begin(_baud, SERIAL_8N1, _rxpin, _txpin);
#else
  (void)_rxpin;
  (void)_txpin;
  _port->begin(_baud);
#endif
}

bool DFRobot_HallJoyStick_UART::begin(void)
{
  uint16_t id[2] = {0, 0};

  if (_stream == NULL) {
    return false;
  }
  openPort();
  setTimeoutTimeMs(100);
  delay(10);
  if (readInputRegister(HALL_JOYSTICK_UART_ADDRESS, HALL_JOYSTICK_REG_PID, id, 2) != 0) {
    return false;
  }
  return id[0] == HALL_JOYSTICK_PID && id[1] == HALL_JOYSTICK_VID;
}

int DFRobot_HallJoyStick_UART::getJoystickXYData(uint16_t *rawX, uint16_t *rawY)
{
  uint16_t raw[3] = {0, 0, 0};

  _hasCachedZ = false;
  if (rawX == NULL || rawY == NULL) {
    return 1;
  }
  if (readInputRegister(HALL_JOYSTICK_UART_ADDRESS, HALL_JOYSTICK_REG_X, raw, 3) != 0) {
    return 1;
  }
  *rawX = raw[0];
  *rawY = raw[1];
  _cachedZ = raw[2];
  _hasCachedZ = true;
  return 0;
}

bool DFRobot_HallJoyStick_UART::getJoystickZKeyState(void)
{
  uint16_t z = 0;

  if (_hasCachedZ) {
    _hasCachedZ = false;
    return _cachedZ == eHallKeyPressed;
  }
  if (readInputRegister(HALL_JOYSTICK_UART_ADDRESS, HALL_JOYSTICK_REG_Z, &z, 1) != 0) {
    return false;
  }
  return z == eHallKeyPressed;
}
