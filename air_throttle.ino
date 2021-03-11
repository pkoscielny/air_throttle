/*  
 * Hardware:
 * - board SparkFun Pro Micro 5V 16MHz.
 * - step motor 28BYJ-48 with ULN2003 driver
 * - hall sensor ACS712 - 20A.
 * 
*/

#include <ACS712.h>
#include <Stepper.h>

// Motor.
const int steps = 32;             // the number of motor steps.
const int rotate_360 = 2048;      // 32*64 - because of gear.
Stepper motor(steps, 6, 8, 7, 9); // pay attention to pins order. Current order is proper.

// HALL effect sensor.
// SparFun has 5.0 volt with a max value of 1023 steps.
// ACS712 5A  uses 185 mV per A.
// ACS712 20A uses 100 mV per A.  <<-----------
// ACS712 30A uses  66 mV per A.
ACS712 acs(A0, 5.0, 1023, 100);


void setup() {
  // Init rotor.
  motor.setSpeed(800);

  // Init HALL sensor.
  acs.autoMidPoint();
}


bool is_stove_running   = false;
bool is_throttle_opened = false;
int mA; // the value taken from Hall sensor (in miliampers).

void loop() {
  // Read current.
  mA = acs.mA_AC();
  //Serial.println(mA);

  // Stove is running.
  is_stove_running = (mA > 200) ? true : false; 

  // Run motor if needed.
  if (is_stove_running && !is_throttle_opened) {
    open_throttle(motor);
    is_throttle_opened = true;
  } 
  else if (!is_stove_running && is_throttle_opened) {
    close_throttle(motor);
    is_throttle_opened = false;
  }
  
  delay(500);
}


void open_throttle (Stepper motor) {
  motor.step(rotate_360/4); // 512 == turn 90 deg.
}

void close_throttle (Stepper motor) {
  motor.step(-rotate_360/4); // 512 == turn 90 deg.
}

