#ifndef MICROLINK_H
#define MICROLINK_H

#include <Arduino.h>

#define DEVICE_NAME "CodeCell"
#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define GENERAL_SERVICE_UUID "12345678-1234-1234-1234-123456789015"
#define JOYSTICK_UUID "abcd1234-abcd-1234-abcd-123456789012"
#define BATTERY_UUID "56781234-5678-5678-5678-567812345671"
#define BUTTONS_UUID "dcba4328-dcba-4321-dcba-432123456789"
#define SLIDER_UUID "dcba4330-dcba-4321-dcba-432123456790"
#define STRING_UUID "dcba4330-dcba-4321-dcba-432123456791"
#define METRICS_UUID "dcba4322-dcba-4321-dcba-432123456789"
#define SETTINGS_UUID "dcba4330-dcba-4321-dcba-432123456789"
#define ENABLE_UUID "dcba4333-dcba-4321-dcba-432123456789"

#define EN_JOYSTICK 0x01
#define EN_BUTTON_A 0x02
#define EN_BUTTON_B 0x04
#define EN_BUTTON_C 0x08
#define EN_BUTTON_D 0x10
#define EN_SLIDER1 0x20
#define EN_SLIDER2 0x40
#define EN_SLIDER3 0x80

struct MetricsPacket {
  uint16_t heading;
  uint16_t proximity;
};

class MicroLink {
public:
  MicroLink();
  void Init();
  void Print(char string[]);
  void ShowSensors(uint8_t battery_val, uint16_t proximity_val, float heading_val);
  bool ReadButtonA();
  bool ReadButtonB();
  bool ReadButtonC();
  bool ReadButtonD();
  bool ReadButtonJoystick();
  uint8_t ReadSlider1();
  uint8_t ReadSlider2();
  uint8_t ReadSlider3();
  uint8_t ReadJoystickX();
  uint8_t ReadJoystickY();

private:
  uint8_t _currentEnableMask = 0;
  char *_mesage_last = nullptr;
  uint8_t _lastEnableMask = 0;
  void enableScreen(uint8_t bitMask);
};

#endif
