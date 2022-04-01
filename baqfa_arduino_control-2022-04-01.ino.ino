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

int pos1 = 0; // positioning of first servo
int pos2 = 90; // use for positioning second servo

// the interval between pictures during BaQFA, 5000ms are needed to take the picture 
int interval = 1795000;

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

  delay(20000); // wait 30 seconds so plate can be put in place

  close_lid();

  pinMode(A4 , OUTPUT); // define the A4 analog pin for the camera shutter control

  testpicture(3);
  
  delay(60000); // wait 1 minute until BaQFA starts so the box is closed for sure
  
}




void loop() {

    makepicture();
    delay(interval);
  
}




void open_lid() {
    
    servo1.attach(2);
    servo2.attach(3);
    delay(50);
    pos2 = 0;
  
    //open lid at speed of 1° every 15ms
    for (pos1 = 90; pos1 >= 0; pos1--) {
    servo1.write(pos1);
      servo2.write(pos2);
      pos2++;
      delay(15);
    }
  
    delay(50);
    servo1.detach();
    servo2.detach();
  
}



void close_lid() {
  
    servo1.attach(2);
    servo2.attach(3);
    delay(50);
    pos2 = 90;
  
    //close lid at speed of 1° every 15ms
    for (pos1 = 0; pos1 <= 90; pos1++) {
    servo1.write(pos1);
      servo2.write(pos2);
      pos2--;
      delay(15);
    }
    
    delay(50);
    servo1.detach();
    servo2.detach();
  
}


void lights_on() {
 
    // turn all LEDs on in cold white (4000 K) at half power
    strip.fill(coldwhite);
    strip.show();
    delay(100);
  
}


void lights_off() {
  
  delay(50);
  // turn lights off 
    strip.clear();
    strip.show();
  
}



void makepicture() {
 
  lights_on();
  
    open_lid();
    
    digitalWrite(A4 , HIGH); //trigger the OptoCoupler for camera shutter
    delay(500);
    digitalWrite(A4 , LOW); //stop triggering OptoCoupler
    delay(4500); //wait until exposure time for low-light picture is over
  
  close_lid();
    
    lights_off();
}


void testpicture(int tstp) {

  while (tstp > 0) {
      delay(3000);
        makepicture();
      tstp--;
}
  
}

  
  
  
  
