# CodeCell MicroLink
<img src="https://cdn.shopify.com/s/files/1/0766/9855/0605/files/microlink_05f73ef1-0ef2-45cc-bac0-16b957ae3487.png?v=1745994603" alt="CodeCell MicroLink" width="300" align="right" style="margin-left: 20px;">
This library is made for the CodeCell module. It allows you to wirelessly connect your CodeCell to the MicroLink mobile app, enabling live interaction through sliders, buttons, joystick, and real-time sensory feedback — ideal for controlling tiny robots, diy sensor, or other interactive projects.

## Features

- Bluetooth connection with the free MicroLink app  
- Read joystick, slider, and button inputs  
- Send battery level, proximity, and heading values to your phone  
- Print custom strings or metrics for debugging or user display  


## How It Works

### `myMicroLink.Init()`
Initializes BLE services and characteristics:
- Setup the 4 Buttons, 3 sliders and the joystick 
- Setup up the Battery, proximity, heading, Metrics, messages, and device settings, which are notified from device
  
Also starts advertising your CodeCell for connection.

### `myMicroLink.ShowSensors(uint8_t battery, uint16_t proximity, float heading)`
Sends metrics packet to the app:
- `battery`: battery percentage (0–100)
- `proximity`: proximity sensor value
- `heading`: heading in degrees (adjusted to 0–360° scale)

Only sends if values have changed since the last update to reduce BLE traffic.

### `myMicroLink.Print(char *message)`
Sends a string message (such as debug output) to be displayed in the app.

### Controls
Each of these functions returns a value based on the activity in the app. The buttons, sliders, and joystick are automatically enabled in the app based on which functions are used in the CodeCell project.

- `uint8_t ReadJoystickX()`
- `uint8_t ReadJoystickY()`
- `uint8_t ReadSlider1()`
- `uint8_t ReadSlider2()`
- `uint8_t ReadSlider3()`
- `bool ReadButtonA()`
- `bool ReadButtonB()`
- `bool ReadButtonC()`
- `bool ReadButtonD()`
- `bool ReadButtonJoystick()`

## Example Use Case

```cpp
if (micro.ReadButtonA()) {
  Serial.println("Button A pressed!");
}
```

## MicroLink App

The MicroLink app connects to CodeCell over Bluetooth, allowing you to view live data and link controls to actions through this library.

The MicroLink app is completely free to use and features a clean, ad-free interface.

## License

This library is released under the Apache License 2.0 License and is free to use in personal projects. It uses the `BLEDevice.h` and `BLE2902.h`, licensed under the Apache 2.0 License.

