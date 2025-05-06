/*
 * Overview:
 * This example demonstrates how to use CodeCell with the MicroLink library.
 * In this example, we initialize two DriveCells and use two sliders on the MicroLink to control their  
 * PWM duty cycle. A button is also used to flip the driving polarity.
 * 
 * Connect a DC motor to the DriveCell to vary its speed, or a magnetic actuator to vary its position.
 *
 * Controls:
 * Slider 1 - Controls the PWM duty cycle of the first DriveCell
 * Slider 2 - Controls the PWM duty cycle of the second DriveCell
 * Button A - Flips the magnetic polarity 
 */

#include <DriveCell.h>
#include <CodeCell.h>
#include "MicroLink.h"

// Define the pins connected to the two DriveCells
#define IN1_pin1 2
#define IN1_pin2 3
#define IN2_pin1 5
#define IN2_pin2 6

DriveCell DriveCell1(IN1_pin1, IN1_pin2);
DriveCell DriveCell2(IN2_pin1, IN2_pin2);
CodeCell myCodeCell;
MicroLink myMicroLink;

bool polarity;
char myMessage[18];
float Roll, Pitch, Yaw;

void setup() {
  Serial.begin(115200);  // Start the serial monitor at 115200 baud

  // Initialize CodeCell and enable both light and motion rotation sensors
  myCodeCell.Init(LIGHT + MOTION_ROTATION);

  // Initialize the MicroLink Bluetooth connection using
  myMicroLink.Init();

  // Initialize the two DriveCells
  DriveCell1.Init();
  DriveCell2.Init();
}

void loop() {
  if (myCodeCell.Run(100)) {  // Run the CodeCell update loop every 100Hz

    // Read rotation sensor values: Roll, Pitch, and Yaw
    myCodeCell.Motion_RotationRead(Roll, Pitch, Yaw);

    // Send battery level, proximity, and yaw angle to the MicroLink app
    myMicroLink.ShowSensors(myCodeCell.BatteryLevelRead(), myCodeCell.Light_ProximityRead(), Yaw);

    if (myMicroLink.ReadButtonA()) {
      //Flip driving polairty
      polarity = !polarity;
      myMicroLink.Print("Reversing Polarity");
      delay(1000);
      sprintf(myMessage, "Polarity set to: %u", polarity);
      myMicroLink.Print(myMessage);
      delay(1000);
    } else {
      //Get the Sliders values and output the new dutyclce
      uint8_t slider1 = myMicroLink.ReadSlider1();
      uint8_t slider2 = myMicroLink.ReadSlider2();
      DriveCell1.Drive(polarity, slider1);
      DriveCell2.Drive(polarity, slider2);

      // Send a string message to the MicroLink app
      sprintf(myMessage, "D1: %u%% D2: %u%%", slider1, slider2);
      myMicroLink.Print(myMessage);
    }
  }
}
