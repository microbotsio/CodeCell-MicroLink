/*
 * Overview:
 * This example demonstrates how to use CodeCell with the MicroLink library.
 * It connects to the MicroLink app via Bluetooth, reads button and joystick inputs,
 * and displays them in the Serial Monitor. It also sends sensor data like battery level,
 * proximity, and yaw (rotation) to the app in real-time.
 */

#include <CodeCell.h>     
#include "MicroLink.h"   

CodeCell myCodeCell;      // Create an instance of the CodeCell class
MicroLink myMicroLink;    // Create an instance of the MicroLink class

float Roll, Pitch, Yaw;   // Variables to store rotation data

void setup() {
  Serial.begin(115200); // Start the serial monitor at 115200 baud

  // Initialize CodeCell and enable both light and motion rotation sensors
  myCodeCell.Init(LIGHT + MOTION_ROTATION);

  // Initialize the MicroLink Bluetooth connection using 
  myMicroLink.Init();
}

void loop() {
  if (myCodeCell.Run(100)) {  // Run the CodeCell update loop every 100Hz
    
    // Read rotation sensor values: Roll, Pitch, and Yaw (we'll use Yaw here)
    myCodeCell.Motion_RotationRead(Roll, Pitch, Yaw);

    // Send battery level, proximity, and yaw angle to the MicroLink app
    myMicroLink.ShowSensors(myCodeCell.BatteryLevelRead(), myCodeCell.Light_ProximityRead(), Yaw);

    // Check and print if each button is pressed
    if (myMicroLink.ReadButtonA()) {
      Serial.println("ButtonA Pressed");
    }
    if (myMicroLink.ReadButtonB()) {
      Serial.println("ButtonB Pressed");
    }
    if (myMicroLink.ReadButtonC()) {
      Serial.println("ButtonC Pressed");
    }
    if (myMicroLink.ReadButtonD()) {
      Serial.println("ButtonD Pressed");
    }

    // If joystick is pressed, read and print X and Y values
    if (myMicroLink.ReadButtonJoystick()) {
      Serial.print("Joystick X: ");
      Serial.print(myMicroLink.ReadJoystickX());
      Serial.print(", Y: ");
      Serial.println(myMicroLink.ReadJoystickY());
    }

    // Send a string message to the MicroLink app 
    myMicroLink.Print("Hello World");
  }
}
