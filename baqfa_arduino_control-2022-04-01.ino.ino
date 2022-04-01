/*
Control the BaQFA setup for 1 plate with an Arduino Uno, using an optocoupler 
for the camera shutter control, two SG90 servo motors, and a WS2812B LED Strip
ligthing.
*/


#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define LED_PIN     12
#define LED_COUNT    20 //define the real number of LEDs according to the setup

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Servo servo1; // right side servo when looking from the direction of the camera
Servo servo2; // left side servo when looking from the direction of the camera

//colors for cold white 4000K
uint32_t coldwhite = strip.Color(128, 105, 82);


void setup() {

  strip.begin();
  strip.show();
  
  servo1.attach(2);
  servo2.attach(3);
  //delay(300);

  // open lid
  servo1.write(0);
  servo2.write(90);

  delay(30000); // wait 30 seconds so plate can be put in place

  //close lid
  servo1.write(90);
  servo2.write(0);

  delay(300);
  
  // detach servos to prevent them from moving when LED switches on/off
  servo1.detach();
  servo2.detach();

  pinMode(A4 , OUTPUT); // define the A4 analog pin for the camera shutter control

  delay(1000);

  testpicture();
    
}




void loop() {

// turn all LEDs on in cold white (4000 K) at half power
  strip.fill(coldwhite);
  strip.show();

  servo1.attach(2);
  servo2.attach(3);
  delay(500);

  // Open the lid
  servo1.write(0);
  servo2.write(90);
  delay(2000);  //wait until the lid is open for sure and stops wobbling
  
  digitalWrite(A4 , HIGH); //trigger the OptoCoupler for camera shutter
  delay(500);
  digitalWrite(A4 , LOW); //stop triggering OptoCoupler
  delay(5000);
  
  servo1.write(90);
  servo2.write(0);

  delay(1000);

  servo1.detach();
  servo2.detach();

  
  delay(500);

// turn lights off 
  strip.clear();
  strip.show();

   
   delay(1790500);  //wait for a total cycle time of 30min
  
}


void testpicture() {

  int tstp = 0;
  
  while (tstp < 2) {

  // turn all LEDs on in cold white (4000 K) at half power
    strip.fill(coldwhite);
    strip.show();

    servo1.attach(2);
    servo2.attach(3);
    delay(500);

    // Open the lid
    servo1.write(0);
    servo2.write(90);
    delay(2000);  //wait until the lid is open for sure and stops wobbling
  
    digitalWrite(A4 , HIGH); //trigger the OptoCoupler for camera shutter
    delay(500);
    digitalWrite(A4 , LOW); //stop triggering OptoCoupler
    delay(4000);
  
    servo1.write(90);
    servo2.write(0);

    delay(1000);

    servo1.detach();
    servo2.detach();

  
    delay(500);

  // turn lights off 
    strip.clear();
    strip.show();

   
    delay(3000);

    tstp++;
}
  
}
