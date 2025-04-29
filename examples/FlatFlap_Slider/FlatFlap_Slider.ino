/*
 * Overview:
 * This example demonstrates how to use CodeCell with the MicroLink library.
 * In this example, we initialize two DriveCells driving two FlatFlaps and use two sliders on the MicroLink to control their duty cycle and speed. 
 * The buttons are also used to flip between different modes: angle control, speed control, buzzing and flipping the polarity
 */

#include <DriveCell.h>     // Include the library for controlling DriveCell modules
#include <CodeCell.h>      // Include the library for interacting with CodeCell
#include "MicroLink.h"     // Include the MicroLink library for Bluetooth communication

// Define the pins connected to the DriveCell modules
#define IN1_pin1 2
#define IN1_pin2 3
#define IN2_pin1 5
#define IN2_pin2 6

DriveCell FlatFlap1(IN1_pin1, IN1_pin2);
DriveCell FlatFlap2(IN2_pin1, IN2_pin2);

CodeCell myCodeCell;

MicroLink myMicroLink;

// State and data variables
bool flap_polarity = false, flap_state = 1;            
uint16_t flap_slider1 = 0, flap_slider2 = 0;  

char myMessage[22];               

void setup() {
  Serial.begin(115200);  // Start the serial monitor for debugging

  // Initialize the CodeCell with both light and motion rotation sensors
  myCodeCell.Init(LIGHT + MOTION_ROTATION);

  // Start the MicroLink Bluetooth communication
  myMicroLink.Init();

  // Initialize both DriveCells connected to the FlatFlaps
  FlatFlap1.Init();
  FlatFlap2.Init();
}

void loop() {

  if (myCodeCell.Run(10)) {  // Run CodeCell update loop every 10Hz
    
    // Read light sensor proximity and normalize the value
    uint16_t proximity = myCodeCell.Light_ProximityRead();
    proximity = proximity / 20;      // Scale it down
    if (proximity > 100) proximity = 100;

    // Send sensor data (battery and proximity) to the MicroLink app
    myMicroLink.ShowSensors(myCodeCell.BatteryLevelRead(), proximity, 0);

    // Read the values from the two sliders in the app
    flap_slider1 = myMicroLink.ReadSlider1();
    flap_slider2 = myMicroLink.ReadSlider2();

    // Check if Button A was pressed - switch to angle control mode
    if (myMicroLink.ReadButtonA()) {
      flap_state = 1;
      myMicroLink.Print("Flap Angle Control");
      delay(1000);

    // Check if Button B was pressed - switch to speed control mode
    } else if (myMicroLink.ReadButtonB()) {
      flap_state = 0;
      myMicroLink.Print("Flap Speed Control");
      delay(1000);

    // Check if Button C was pressed - activate buzzing on both flaps
    } else if (myMicroLink.ReadButtonC()) {
      FlatFlap1.Tone();
      FlatFlap2.Tone();
      myMicroLink.Print("Flap Buzzing");

    // Check if Button D was pressed - reverse driving polarity
    } else if (myMicroLink.ReadButtonD()) {
      flap_polarity = !flap_polarity;  // Flip polarity boolean
      myMicroLink.Print("Reversing Polarity");

      // Stop both motors briefly
      FlatFlap1.Drive(flap_polarity, 0);
      FlatFlap2.Drive(flap_polarity, 0);
      delay(1000);

      // Show updated polarity state
      sprintf(myMessage, "Polarity set to: %u", flap_polarity);
      myMicroLink.Print(myMessage);
      delay(1000);

    } else {
      // If no button is pressed, act based on current flap_state

      // -------- ANGLE CONTROL MODE --------
      if (flap_state == 1) {
        // Reduce slider values by proximity to account for hand distance
        flap_slider2 = (flap_slider2 > proximity) ? flap_slider2 - proximity : 0;
        flap_slider1 = (flap_slider1 > proximity) ? flap_slider1 - proximity : 0;

        // Send PWM drive signals to each FlatFlap
        FlatFlap1.Drive(flap_polarity, flap_slider1);
        FlatFlap2.Drive(flap_polarity, flap_slider2);

        // Display duty cycle percentage on the MicroLink app
        sprintf(myMessage, "D1: %u%% D2: %u%%", flap_slider1, flap_slider2);
        myMicroLink.Print(myMessage);

      // -------- SPEED CONTROL MODE --------
      } else if (flap_state == 0) {
        // Multiply slider value by 10 to convert to milliseconds
        flap_slider1 = flap_slider1 * 10;
        flap_slider2 = flap_slider2 * 10;

        // Run flaps in time-based speed mode
        FlatFlap1.Run(true, 100, flap_slider1);
        FlatFlap2.Run(true, 100, flap_slider2);

        // Show timing values in the app
        sprintf(myMessage, "S1: %ums S2: %ums", flap_slider1, flap_slider2);
        myMicroLink.Print(myMessage);
      }
    }
  }
}
