/*!
 * @file readJoystickUart.ino
 * @brief Read the hall joystick over UART
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-22
 * @url https://github.com/DFRobot/DFRobot_HallJoyStick
 */

/*
 * Gravity UART, 9600 8N1. Module PC3 is TX, module PC4 is RX.
 * COM_SEL must be low (UART). The slave address stays 0x33.
 * USB serial prints at 115200. The module port is separate.
 *
 * Mega 2560 Serial1: MCU RX 19, MCU TX 18
 * UNO SoftwareSerial: MCU RX 10, MCU TX 11
 */

#include "DFRobot_HallJoyStick.h"

#if defined(ESP8266) || defined(ARDUINO_AVR_UNO)
#include <SoftwareSerial.h>
SoftwareSerial softSerial(10, 11);
DFRobot_HallJoyStick_UART joystick(&softSerial, HALL_JOYSTICK_UART_BAUD);
#else
DFRobot_HallJoyStick_UART joystick(&Serial1, HALL_JOYSTICK_UART_BAUD);
#endif

void printSample(void)
{
  uint16_t x = 0;
  uint16_t y = 0;

  if (joystick.getJoystickXYData(&x, &y) == 0) {
    Serial.print("X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
  } else {
    Serial.print("XY read failed");
  }
  Serial.print(" Z=");
  Serial.println(joystick.getJoystickZKeyState() ? "pressed" : "released");
}

void setup(void)
{
  Serial.begin(115200);
  while (joystick.begin() == false) {
    Serial.println("begin failed");
    delay(1000);
  }
  Serial.println("begin ok");
  printSample();
}

void loop(void)
{
  printSample();
}
