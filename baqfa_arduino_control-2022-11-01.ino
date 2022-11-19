/*
Control the BaQFA setup for 1 plate with an Arduino Uno, using an optocoupler 
for the camera shutter control, two SG90 servo motors, and a WS2812B LED Strip
ligthing.
*/


#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define LED_PIN     12
#define LED_COUNT    52 //define the real number of LEDs according to the setup

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Servo servo_R; // right side servo when looking from the direction of the camera
Servo servo_L; // left side servo when looking from the direction of the camera

// servo positioning
int open_position = 100;
int close_position = 7;
int pos_open_R = close_position - 5; // open positioning of right servo adjusting for servo offest due to mechanical parts
int pos_open_L = open_position - 10; // open positioning left servo adjusting for servo offest due to mechanical parts
int pos_close_R = open_position + 3; // close positioning of right servo adjusting for servo offest due to mechanical parts
int pos_close_L = close_position - 2; // close positioning of left servo adjusting for servo offest due to mechanical parts

int pos_current_R = pos_open_R;
int pos_current_L = pos_open_L;

// the interval between pictures during BaQFA, 5000ms are needed to take the picture 
int interval = 1795000;

//colors for cold white 4000K
uint32_t coldwhite = strip.Color(128, 105, 82);
uint32_t dark = strip.Color(0, 0, 0);


void setup() {

  strip.begin();
  strip.show();

  pinMode(A4 , OUTPUT); // define the A4 analog pin for the camera shutter control
  
  servo_R.attach(2);
  servo_L.attach(3);
  delay(100);

  // open lid
  servo_R.write(pos_current_R);
  servo_L.write(pos_current_L);
  delay(500);
  servo_R.detach();
  servo_L.detach();
  delay(1000);

  lights_on();

  delay(20000); // wait 20 seconds so plate can be put in place

  lights_off();
  delay(1000);

  close_lid();
  delay(3000);

  testpicture(3);
  
  delay(60000); // wait 1 minute until BaQFA starts so the box is closed for sure
  
}




void loop() {

    makepicture();
    delay(1795000);
  
}




void open_lid() {
    
    servo_R.attach(2);
    servo_L.attach(3);
    delay(50);
    pos_current_L = pos_close_L -15; // some bug fucks up the corrections from above, thus it is re-applied here as quick-fix...
    
    //open lid at speed of 1° every 5ms
    for (pos_current_R = pos_close_R; pos_current_R >= pos_open_R; pos_current_R--) {
      servo_R.write(pos_current_R);
      servo_L.write(pos_current_L);
      pos_current_L++;
      delay(5);
    }
  
    delay(50);
    servo_R.detach();
    servo_L.detach();
  
}



void close_lid() {
  
    servo_R.attach(2);
    servo_L.attach(3);
    delay(50);
    pos_current_R = pos_open_R;
  
    //close lid at speed of 1° every 7ms
    for (pos_current_L = pos_open_L; pos_current_L >= pos_close_L; pos_current_L--) {
      servo_R.write(pos_current_R);
      servo_L.write(pos_current_L);
      pos_current_R++;
      delay(7);
    }
    
    delay(50);
    servo_R.detach();
    servo_L.detach();
  
}


void lights_on() {
 
    // turn all LEDs on in cold white (4000 K) at half power
    //strip.fill(coldwhite);
    for (int c = 1; c <= LED_COUNT; c++) {
    strip.setPixelColor(c, coldwhite);
    strip.show();
    delay(18);
    }
    delay(100);
  
}


void lights_off() {
  
  delay(50);
  // turn lights off 
    strip.clear();
    strip.show();
  
}



void makepicture() {

    open_lid();
    delay(1000);
    lights_on();
    delay(500);
    digitalWrite(A4 , HIGH); //trigger the OptoCoupler for camera shutter
    delay(1000);
    digitalWrite(A4 , LOW); //stop triggering OptoCoupler
    delay(4500); //wait until exposure time for low-light picture is over  
    lights_off();
    delay(1000);
    close_lid();
}


void testpicture(int tstp) {

  while (tstp > 0) {
      delay(3000);
        makepicture();
      tstp--;
}
  
}

  
  
  
  
