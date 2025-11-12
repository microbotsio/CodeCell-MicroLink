#include "MicroLink.h"
#include <BLEDevice.h>
#include <BLE2902.h>

BLECharacteristic *pJoystickCharacteristic = NULL;
BLECharacteristic *pBatteryCharacteristic = NULL;
BLECharacteristic *pButtonCharacteristic = NULL;
BLECharacteristic *pSliderCharacteristic = NULL;
BLECharacteristic *pStringCharacteristic = NULL;
BLECharacteristic *pMetricsCharacteristic = NULL;
BLECharacteristic *pSettingsCharacteristic = NULL;
BLECharacteristic *pEnableCharacteristic = NULL;

uint8_t rx_joystick_x = 100, rx_joystick_y = 100, slider1 = 0, slider2 = 0, slider3 = 0, last_battery_val = 255;
bool ButtonA = 0, ButtonB = 0, ButtonC = 0, ButtonD = 0, ButtonJoystick = 0, isAppConnectionReady = 0;

MetricsPacket last_metrics = { 0 };

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();

    if ((pCharacteristic == pJoystickCharacteristic) && (value.length() > 0)) {
      rx_joystick_y = static_cast<uint8_t>(value[1]);  // Joystick Y for speed
      rx_joystick_y = constrain(rx_joystick_y, 0, 200);
      rx_joystick_x = static_cast<uint8_t>(value[0]);  // Joystick X for direction
      rx_joystick_x = constrain(rx_joystick_x, 0, 200);
    } else if ((pCharacteristic == pButtonCharacteristic) && (value.length() > 0)) {
      uint8_t button_value = static_cast<uint8_t>(value[0]);
      ButtonJoystick = (bool)(button_value & 0x1);
      ButtonA = (bool)((button_value >> 1) & 0x1);
      ButtonB = (bool)((button_value >> 2) & 0x1);
      ButtonC = (bool)((button_value >> 3) & 0x1);
      ButtonD = (bool)((button_value >> 4) & 0x1);
    } else if ((pCharacteristic == pSliderCharacteristic) && (value.length() > 0)) {
      slider1 = static_cast<uint8_t>(value[0]);
      slider2 = static_cast<uint8_t>(value[1]);
      slider3 = static_cast<uint8_t>(value[2]);
    } else if (pCharacteristic == pSettingsCharacteristic && value.length() > 0) {
      isAppConnectionReady = static_cast<uint8_t>(value[0]);
    } else {
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    Serial.println(">> MicroLink: Connected to App");
    delay(1000);
  }

  void onDisconnect(BLEServer *pServer) override {
    Serial.println(">> MicroLink: Disconnected from App");
    delay(500);                     // Small delay to ensure proper disconnection before re-advertising
    BLEDevice::startAdvertising();  // Restart advertising so new devices can connect
    Serial.println(">> MicroLink: Bluetooth Advertising..");
    isAppConnectionReady = false;
  }
};

MicroLink::MicroLink() {
}
void MicroLink::Init() {
  BLEDevice::init(DEVICE_NAME);
  BLEServer *bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new MyServerCallbacks());

  BLEService *bleService = bleServer->createService(BLEUUID(SERVICE_UUID), 30, 0);
  BLEService *bleGeneralService = bleServer->createService(GENERAL_SERVICE_UUID);

  pJoystickCharacteristic = bleService->createCharacteristic(JOYSTICK_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pJoystickCharacteristic->addDescriptor(new BLE2902()); /*Add description to client configuration*/
  pJoystickCharacteristic->setCallbacks(new MyCallbacks());

  pBatteryCharacteristic = bleGeneralService->createCharacteristic(BATTERY_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pBatteryCharacteristic->addDescriptor(new BLE2902()); /*Add description to client configuration*/
  pBatteryCharacteristic->setCallbacks(new MyCallbacks());

  pStringCharacteristic = bleService->createCharacteristic(STRING_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pStringCharacteristic->addDescriptor(new BLE2902()); /*Add description to client configuration*/
  pStringCharacteristic->setCallbacks(new MyCallbacks());

  pButtonCharacteristic = bleService->createCharacteristic(BUTTONS_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pButtonCharacteristic->addDescriptor(new BLE2902()); /*Add description to client configuration*/
  pButtonCharacteristic->setCallbacks(new MyCallbacks());

  pSliderCharacteristic = bleService->createCharacteristic(SLIDER_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pSliderCharacteristic->addDescriptor(new BLE2902()); /*Add description to client configuration*/
  pSliderCharacteristic->setCallbacks(new MyCallbacks());

  pMetricsCharacteristic = bleService->createCharacteristic(METRICS_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pMetricsCharacteristic->addDescriptor(new BLE2902());

  pSettingsCharacteristic = bleService->createCharacteristic(SETTINGS_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  pSettingsCharacteristic->addDescriptor(new BLE2902()); /*Add description to client configuration*/
  pSettingsCharacteristic->setCallbacks(new MyCallbacks());

  pEnableCharacteristic = bleService->createCharacteristic(ENABLE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pEnableCharacteristic->addDescriptor(new BLE2902()); /*Add description to client configuration*/

  bleService->start();
  bleGeneralService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->addServiceUUID(GENERAL_SERVICE_UUID);
  BLEDevice::startAdvertising();
  Serial.println(">> MicroLink: Bluetooth Advertising..");
}

void MicroLink::ShowSensors(uint8_t battery_val, uint16_t proximity_val, float heading_val) {
  uint16_t uheading_val = 0;
  if (isAppConnectionReady) {
    uheading_val = (uint16_t)(heading_val + 180);

    if (battery_val != last_battery_val) {
      pBatteryCharacteristic->setValue(&battery_val, 1);
      pBatteryCharacteristic->notify();
    }
    last_battery_val = battery_val;

    MetricsPacket metrics;
    metrics.heading = uheading_val;
    metrics.proximity = proximity_val;

    if (memcmp(&metrics, &last_metrics, sizeof(MetricsPacket)) != 0) {
      pMetricsCharacteristic->setValue((uint8_t *)&metrics, sizeof(MetricsPacket));
      pMetricsCharacteristic->notify();
      last_metrics = metrics;  // Copy new values to previous
    }
  }
}

void MicroLink::Print(char *mesage) {
  if ((_mesage_last == nullptr) || (strcmp(mesage, _mesage_last) != 0)) {
    size_t len = strlen(mesage);
    pStringCharacteristic->setValue((uint8_t *)mesage, len);
    pStringCharacteristic->notify();

    _mesage_last = strdup(mesage);
  }
}

void MicroLink::enableScreen(uint8_t bitMask) {
  _currentEnableMask |= bitMask;
  if (_currentEnableMask != _lastEnableMask) {
    pEnableCharacteristic->setValue(&_currentEnableMask, 1);
    pEnableCharacteristic->notify();
    _lastEnableMask = _currentEnableMask;
  }
}

uint8_t MicroLink::ReadJoystickX() {
  enableScreen(EN_JOYSTICK);
  return rx_joystick_x;
}
uint8_t MicroLink::ReadJoystickY() {
  enableScreen(EN_JOYSTICK);
  return rx_joystick_y;
}
uint8_t MicroLink::ReadSlider1() {
  enableScreen(EN_SLIDER1);
  return slider1;
}
uint8_t MicroLink::ReadSlider2() {
  enableScreen(EN_SLIDER2);
  return slider2;
}
uint8_t MicroLink::ReadSlider3() {
  enableScreen(EN_SLIDER3);
  return slider3;
}
bool MicroLink::ReadButtonA() {
  enableScreen(EN_BUTTON_A);
  return ButtonA;
}
bool MicroLink::ReadButtonB() {
  enableScreen(EN_BUTTON_B);
  return ButtonB;
}
bool MicroLink::ReadButtonC() {
  enableScreen(EN_BUTTON_C);
  return ButtonC;
}
bool MicroLink::ReadButtonD() {
  enableScreen(EN_BUTTON_D);
  return ButtonD;
}
bool MicroLink::ReadButtonJoystick() {
  enableScreen(EN_JOYSTICK);
  return ButtonJoystick;
}
bool MicroLink::ReadConnect() {
  return isAppConnectionReady;
}

