/*
 * Overview:
 * This example demonstrates how to use CodeCell with the MicroLink library.
 * It shows how to control two motors using two DriveCells.
 * The user controls motor speed and direction using a joystick in the MicroLink app.
 */

#include <DriveCell.h>     
#include <CodeCell.h>     
#include "MicroLink.h"     

// Define which pins on the CodeCell are connected to each motor driver
#define IN1_pin1 2
#define IN1_pin2 3
#define IN2_pin1 5
#define IN2_pin2 6

DriveCell DriveCell1(IN1_pin1, IN1_pin2);
DriveCell DriveCell2(IN2_pin1, IN2_pin2);

CodeCell myCodeCell;
MicroLink myMicroLink;

// Variables for motion sensor data
float Roll, Pitch, Yaw;

// Variables to store motor direction and speed
bool motor1_dir = 0, motor2_dir = 0;
uint8_t motor1_spd = 0, motor2_spd = 0;

char myMessage[18];  

void setup() {
  Serial.begin(115200);  // Start the serial monitor at 115200 baud

  // Initialize CodeCell and enable both light and motion rotation sensors
  myCodeCell.Init(LIGHT + MOTION_ROTATION);

  // Initialize the MicroLink Bluetooth connection using
  myMicroLink.Init();

  // Initialize the two motors DriveCells
  DriveCell1.Init();
  DriveCell2.Init();
}

void loop() {
  if (myCodeCell.Run(100)) {  // Run the CodeCell update loop every 100Hz

    // Read rotation data from the motion sensor
    myCodeCell.Motion_RotationRead(Roll, Pitch, Yaw);

    // Show sensor readings (battery level, light sensor, yaw) in the app
    myMicroLink.ShowSensors(myCodeCell.BatteryLevelRead(), myCodeCell.Light_ProximityRead(), Yaw);

    // Check if joystick is being used
    if (myMicroLink.ReadButtonJoystick()) {
      // Read joystick's X and Y values and shift to center them around 0
      int dx = myMicroLink.ReadJoystickX() - 100;
      int dy = myMicroLink.ReadJoystickY() - 100;

      // Calculate how far the joystick is pushed (speed) and in what angle (direction)
      float speed = sqrt(((dx * dx) + (dy * dy)));           // Joystick distance from center
      float angle = (atan2(dy, dx) * (180.0 / M_PI)) + 180;  // Angle in degrees (0–360)

      uint16_t speed_control = 0;
      // Determine direction: forward (angle < 180) or reverse
      if (angle < 180) {
        motor1_dir = 0;
        motor2_dir = 0;
        speed_control = (uint16_t)abs(angle - 90);
      } else {
        motor1_dir = 1;
        motor2_dir = 1;
        speed_control = (uint16_t)abs(angle - 270);
      }

      // Adjust motor speeds based on joystick angle
      if ((angle <= 90) || (angle >= 270)) {
        motor1_spd = (uint8_t)speed;
        if (speed > speed_control) {
          speed = (speed - speed_control);
        } else {
          speed = 0;
        }
        motor2_spd = (uint8_t)speed;
      } else {
        motor2_spd = (uint8_t)speed;
        if (speed > speed_control) {
          speed = (speed - speed_control);
        } else {
          speed = 0;
        }
        motor1_spd = (uint8_t)speed;
      }

      // Drive the motors with the calculated speed
      DriveCell2.Drive(motor2_dir, motor2_spd);
      DriveCell1.Drive(motor1_dir, motor1_spd);

      // Send a message to the app showing motor speeds percentage
      sprintf(myMessage, "M1: %u%% M2: %u%%", motor1_spd, motor2_spd);
      myMicroLink.Print(myMessage);
    }
  }
}
