/*
 * Overview:
 * This example demonstrates how to use CodeCell with the MicroLink library.
 * In this example, we initialize a MotorCell and use a slider on the MicroLink to control its speed.
 * The RPM value is printed on the screen and a button is also used to reverse the motor.
 *
 */


#include <MotorCell.h>
#include <CodeCell.h>
#include "MicroLink.h"

#define IN_pin1 2
#define OUT_pin2 3
#define FR_pin2 1

MotorCell myMotorCell(IN_pin1, OUT_pin2, FR_pin2); /*Configure the pins used for the MotorCell */
CodeCell myCodeCell;
MicroLink myMicroLink;

bool polarity;
char message[18];

void setup() {
  Serial.begin(115200);  // Start the serial monitor at 115200 baud

  // Initialize CodeCell light sensor
  myCodeCell.Init(LIGHT);

  // Initialize the MicroLink Bluetooth connection using
  myMicroLink.Init();

  myMotorCell.Init(); /* Initialize the MotorCell */
}

void loop() {
  if (myCodeCell.Run(10)) {  // Run the CodeCell update loop every 10Hz

    // Send battery level and proximity to the MicroLink app
    myMicroLink.ShowSensors(myCodeCell.BatteryLevelRead(), myCodeCell.Light_ProximityRead(), 0);

    if (myMicroLink.ReadButtonA()) {
      //Flip driving polairty
      polarity = !polarity;
      myMotorCell.ReverseSpin();
      myMicroLink.Print("Reversing Polarity");
      delay(1000);
      sprintf(message, "Polarity set to: %u", polarity);
      myMicroLink.Print(message);
      delay(1000);
    } else {
      //Get the Sliders values and output the new dutyclce
      uint16_t slider1 = (uint16_t)(myMicroLink.ReadSlider1())*300;
      
      uint16_t MotorRPM = myMotorCell.SpinPID(slider1); /* Set the target RPM to 15,000 using the PID controller */

      // Send a string message to the MicroLink app
      sprintf(message, "%u RPM", MotorRPM);
      myMicroLink.Print(message);
    }
  }
}
