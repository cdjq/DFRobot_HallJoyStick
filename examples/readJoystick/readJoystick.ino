/*!
 * @file readJoystick.ino
 * @brief Read the hall joystick over I2C
 * @n connected table
 * @n -----------------------------------------------
 * @n  board  |      Mega 2560      |      UNO
 * @n   VCC   |         5V          |      5V
 * @n   GND   |         GND         |      GND
 * @n   SDA   |          20         |      A4
 * @n   SCL   |          21         |      A5
 * @n -----------------------------------------------
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-22
 * @url https://github.com/DFRobot/DFRobot_HallJoyStick
 */
#include "DFRobot_HallJoyStick.h"

DFRobot_HallJoyStick_I2C joystick(&Wire, HALL_JOYSTICK_I2C_ADDRESS_LOW);

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
