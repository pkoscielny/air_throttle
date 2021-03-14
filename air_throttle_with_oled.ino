/*
 * IDE: Arduino IDE 1.8.13.
 * Board: SparkFun Pro Micro.
 *  
 * Hardware:
 * - SparkFun Pro Micro 5V 16MHz board.
 * - stepper motor 28BYJ-48 with ULN2003 driver.
 * - hall sensor ACS712 20A.
 * - 0.96" OLED display.
*/

#include <ACS712.h>
#include <Stepper.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// OLED display.
#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(-1);

// Motor.
const int steps = 32;             // the number of motor steps.
const int rotate_360 = 2048;      // 32*64 - because of gear.
Stepper motor(steps, 6, 8, 7, 9); // pay attention to pins order. Current order is proper.

// Device current threshlod.
const int current_threshold = 200; // mA.

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

  // Init and clear OLED display.
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE);
}


bool is_device_running  = false;
bool is_throttle_opened = false;
int mA; // the value taken from Hall sensor (in miliampers).

void loop() {
  // Read current.
  mA = acs.mA_AC();
  display_hall(mA, is_throttle_opened);
  //Serial.println(mA);

  // Some device is running.
  is_device_running = (mA > current_threshold) ? true : false; 

  // Run motor if needed.
  if (is_device_running && !is_throttle_opened) {
    display_throttle_state("opening...");
    open_throttle(motor);
    is_throttle_opened = true;
  } 
  else if (!is_device_running && is_throttle_opened) {
    display_throttle_state("closing...");
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

// Views definitions.
void display_hall(int hall, bool is_throttle_opened) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Hall [mA]:\n");
  display.setTextSize(2);
  display.print(hall);
  display.print("\n");
  display.setTextSize(1);
  display.print("Throttle: ");
  if (is_throttle_opened) {
    display.print("opened");
  } else {
    display.print("closed");
  }
  display.display();
}

void display_throttle_state(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Throttle:\n");
  display.setTextSize(2);
  display.print(msg);
  display.display();
}

