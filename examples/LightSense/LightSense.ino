/*
 * Overview:
 * This example shows how to use the CodeCell’s light and proximity sensors 
 * with the MicroLink app to interactively control the onboard LED and display sensor data.
 * 
 * It uses a slider to adjust LED brightness and buttons to change LED color and print a message
 * based on light and proximity levels.
 *
 * Controls:
 * Slider 1  - Adjust LED brightness (1–10)
 * Button A  - Toggle LED on/off
 * Button B  - Set LED to red
 * Button C  - Set LED to green
 * Button D  - Print message based on light level
 * Default - Print message based on proximity level
 */

#include <CodeCell.h>
#include "MicroLink.h"

CodeCell myCodeCell;
MicroLink myMicroLink;

bool LED_state = 1;
uint16_t led_slider1 = 100, led_slider1_last = 0;
char myMessage[20];

void setup() {
  Serial.begin(115200);  // Start serial monitor for debugging

  // Initialize CodeCell with both light and rotation sensors
  myCodeCell.Init(LIGHT);

  // Start Bluetooth communication with the MicroLink app
  myMicroLink.Init();
}

void loop() {
  if (myCodeCell.Run(10)) {  // Run CodeCell update loop at 10Hz

    // Read light proximity sensor and normalize the value
    uint16_t proximity = myCodeCell.Light_ProximityRead();
    uint16_t light = myCodeCell.Light_WhiteRead();

    // Send battery level and proximity data to the MicroLink app
    myMicroLink.ShowSensors(myCodeCell.BatteryLevelRead(), proximity, 0);

    // Read slider values from the MicroLink app
    if (LED_state) {
      led_slider1 = myMicroLink.ReadSlider1() / 10;
      if (led_slider1 == 0) {
        led_slider1 = 1;
      } else if (led_slider1 > 10) {
        led_slider1 = 10;
      } else {
        //skip
      }
      if (led_slider1 != led_slider1_last) {
        led_slider1_last = led_slider1;
        myCodeCell.LED_SetBrightness(led_slider1);  // Turn on CodeCell LED
      }
    }

    if (myMicroLink.ReadButtonA()) {
      // Switch to PWM (angle control) mode
      LED_state = !LED_state;
      if (LED_state) {
        myMicroLink.Print("Turning on LED");
        myCodeCell.LED_SetBrightness(led_slider1);  // Turn on CodeCell LED
      } else {
        myMicroLink.Print("Turning off LED");
        myCodeCell.LED_SetBrightness(0);  // Turn off CodeCell LED
      }
      delay(1000);
    } else if (myMicroLink.ReadButtonB()) {
      myMicroLink.Print("Turning LED Red");
      if (!LED_state) {
        myCodeCell.LED_SetBrightness(10);  // Turn on CodeCell LED
      }
      myCodeCell.LED(0xFF, 0, 0);  // Set LED to Red
      delay(1000);
    } else if (myMicroLink.ReadButtonC()) {
      myMicroLink.Print("Turning LED Green");
      if (!LED_state) {
        myCodeCell.LED_SetBrightness(10);  // Turn on CodeCell LED
      }
      myCodeCell.LED(0, 0xFF, 0);  // Set LED to Red
      delay(1000);
    } else if (myMicroLink.ReadButtonD()) {
      if (light < 10) {
        myMicroLink.Print("Lights out..");
      } else if (light < 100) {
        myMicroLink.Print("Getting Dark..");
      } else if (light < 10000) {
        myMicroLink.Print("All lit up..");
      } else {
        myMicroLink.Print("Need sunglasses");
      }
      delay(1000);
    } else {
      if (proximity < 10) {
        myMicroLink.Print("Zone is Clear..");
      } else if (proximity < 10000) {
        myMicroLink.Print("Approaching..");
      } else {
        myMicroLink.Print("Back it up!");
      }
    }
  }
}
