/*
Simplified control script for BaQFA with:
1. Independent opening and closing motion delays for servo speed control.
2. Forcing servo to reach exact intended positions for both opening and closing.
3. 30-minute interval timing using millis().
*/

#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define LED_PIN     12
#define LED_COUNT    52 // Define the real number of LEDs according to the setup

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Servo servo_R; // Right-side servo when looking from the direction of the camera
Servo servo_L; // Left-side servo, inverted when looking from the direction of the camera

// Servo angles: Open and closed positions for both servos
int pos_open_R = 105;  // Open position for right servo
int pos_open_L = 75;   // Open position for left servo (inverted)
int pos_close_R = 0;   // Closed position for right servo
int pos_close_L = 180; // Closed position for left servo (inverted)

// Global variables to track the current servo positions
int pos_current_R = pos_close_R;
int pos_current_L = pos_close_L;

// Interval between pictures during BaQFA (30 minutes in milliseconds)
const unsigned long interval = 1800000; // 30 minutes = 1,800,000 milliseconds

// Time tracking for millis-based interval
unsigned long previous_time = 0; // Stores the last time the loop triggered
bool first_run = true;           // Flag for first loop iteration

// Light colors for cold white: 4000K settings
uint32_t coldwhite = strip.Color(128, 105, 82);
uint32_t dark = strip.Color(0, 0, 0);

// Independent servo delays for opening and closing
int motion_delay_open = 8; // Slightly slower opening (~735ms for 105° motion)
int motion_delay_close = 8; // Faster closing (~525ms for 105° motion)

void setup() {
    // Start Serial Monitor
    Serial.begin(9600);
    while (!Serial);

    Serial.println("Starting BaQFA setup...");

    // Initialize LED strip
    strip.begin();
    strip.show();

    // Define the A4 analog pin for camera shutter control
    pinMode(A4, OUTPUT);

    // Ensure motors start at the closed position
    servo_R.attach(2);
    servo_L.attach(3);

    // Move to closed position (start configuration)
    Serial.println("Setting initial positions to fully closed.");
    servo_R.write(pos_close_R);
    servo_L.write(pos_close_L);
    pos_current_R = pos_close_R;
    pos_current_L = pos_close_L;
    delay(500);

    servo_R.detach();
    servo_L.detach();

    // Initial sequence
    Serial.println("Opening lid to begin setup...");
    open_lid(); // Open lid
    Serial.println("Turning LEDs on for setup.");
    lights_on(); // Turn lights on
    delay(10000); // Wait for setup
    Serial.println("Turning LEDs off after setup.");
    lights_off(); // Turn lights off
    delay(1000);

    Serial.println("Closing lid...");
    close_lid(); // Close lid
    delay(3000);

    Serial.println("Running test pictures...");
    testpicture(3); // Take test pictures to verify behavior

    Serial.println("Finished setup. Entering BaQFA loop after a delay.");
    delay(60000); // Initial delay before loop
}

void loop() {
    unsigned long current_time = millis();

    // Check if it's the first run or if the 30-minute interval has elapsed
    if (first_run || current_time - previous_time >= interval) {
        if (!first_run) { // Skip timer log for the first run
            Serial.print("Elapsed Time: ");
            Serial.print((current_time - previous_time) / 1000);
            Serial.println(" seconds. Taking picture now...");
        }

        first_run = false;

        makepicture(); // Trigger picture-taking process

        // Update the timestamp of the last trigger
        previous_time = millis();
    }

    delay(10); // Short delay to avoid excessive processor workload
}

void open_lid() {
    Serial.println("Opening lid...");
    servo_R.attach(2);
    servo_L.attach(3);
    delay(50);

    int log_counter = 0;

    // Move directly to open positions with adjustable speed
    for (int step = 0; step <= 105; step += 5) { // Move in steps of 5° for smooth transition
        pos_current_R = pos_close_R + step;
        pos_current_L = pos_close_L - step; // Inverted motion for left motor

        servo_R.write(pos_current_R);
        servo_L.write(pos_current_L);

        // Log every 10° increments
        if (step % 10 == 0) {
            logPosition("Opening lid");
        }

        delay(motion_delay_open); // Use delay for speed control
    }

    // Ensure servos reach their final open positions
    servo_R.write(pos_open_R);
    servo_L.write(pos_open_L);

    servo_R.detach();
    servo_L.detach();
    Serial.println("Lid is fully open and servos forced to final position.");
}

void close_lid() {
    Serial.println("Closing lid...");
    servo_R.attach(2);
    servo_L.attach(3);
    delay(50);

    int log_counter = 0;

    // Move directly to closed positions with adjustable speed
    for (int step = 0; step <= 105; step += 15) { // Move in steps of 5° for smooth transition
        pos_current_R = pos_open_R - step;
        pos_current_L = pos_open_L + step; // Inverted motion for left motor

        servo_R.write(pos_current_R);
        servo_L.write(pos_current_L);

        // Log every 10° increments
        if (step % 10 == 0) {
            logPosition("Closing lid");
        }

        delay(motion_delay_close); // Use delay for speed control
    }

    delay(100);
    // Ensure servos reach their final closed positions
    servo_R.write(pos_close_R);
    servo_L.write(pos_close_L);

    servo_R.detach();
    servo_L.detach();
    Serial.println("Lid is fully closed and servos forced to final position.");
}

void logPosition(String message) {
    Serial.print(message + "... pos_current_R: ");
    Serial.print(pos_current_R);
    Serial.print(", pos_current_L: ");
    Serial.println(pos_current_L);
}

void lights_on() {
    Serial.println("Turning lights on...");
    for (int c = 0; c < LED_COUNT; c++) {
        strip.setPixelColor(c, coldwhite);
        strip.show();
        delay(5);
    }
    delay(100);
    Serial.println("Lights are now ON.");
}

void lights_off() {
    Serial.println("Turning lights off...");
    strip.clear();
    strip.show();
    Serial.println("Lights are now OFF.");
}

void makepicture() {
    open_lid();
    delay(1000);
    lights_on();
    delay(500);
    Serial.println("Triggering camera shutter...");
    digitalWrite(A4, HIGH); // Trigger the OptoCoupler for camera shutter
    delay(1000);
    digitalWrite(A4, LOW);  // Stop triggering OptoCoupler
    delay(4500);
    lights_off();
    delay(1000);
    close_lid();
}

void testpicture(int tstp) {
    while (tstp > 0) {
        delay(3000);
        Serial.print("Taking test picture, remaining: ");
        Serial.println(tstp);
        makepicture();
        tstp--;
    }
}