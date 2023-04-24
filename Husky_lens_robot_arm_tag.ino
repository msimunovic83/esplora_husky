#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <HUSKYLENS.h>

// Create HuskyLens object
HUSKYLENS huskylens;

// Create servo driver object
Adafruit_PWMServoDriver servoShield = Adafruit_PWMServoDriver();

// Servo channel assignments
const int servo0Channel = 0;
const int servo1Channel = 1;
const int servo2Channel = 2;
const int servo3Channel = 3;
const int servo4Channel = 4;

// Servo minimum and maximum pulse length in microseconds
const int servoMin = 150;
const int servoMax = 600;

// Movement threshold
const int movementThreshold = 3;

void setup() {
  // Start the shield with the default I2C address (0x40)
  servoShield.begin();
  // Set PWM frequency to 60 Hz
  servoShield.setPWMFreq(60);

  // Start serial communication at 9600 bps
  Serial.begin(9600);

  // Initialize HuskyLens
  Wire.begin();
  huskylens.begin(Wire);
  huskylens.writeAlgorithm(ALGORITHM_TAG_RECOGNITION);
}

void loop() {
  // Request data from HuskyLens
  if (huskylens.request()) {
    if (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();
      // Extract object position
      int objectX = result.xTarget;
      int objectY = result.yTarget;

      // Map object position to servo angles
      int servo0Angle = map(objectX, 0, 320, 30, 150); // Assuming a 320x240 resolution camera
      int servo1Angle = map(objectY, 0, 240, 30, 150);

      // Calculate the difference between the new and the old angles
      static int oldServo0Angle = servo0Angle;
      static int oldServo1Angle = servo1Angle;

      int servo0Diff = abs(servo0Angle - oldServo0Angle);
      int servo1Diff = abs(servo1Angle - oldServo1Angle);

      // Only update servos if the object has moved significantly
      if (servo0Diff > movementThreshold || servo1Diff > movementThreshold) {
        setServoPosition(servo0Channel, servo0Angle);
        setServoPosition(servo1Channel, servo1Angle);

        // Print the servo angles
        Serial.print("Servo0: ");
        Serial.print(servo0Angle);
        Serial.print(" | Servo1: ");
        Serial.println(servo1Angle);

        // Update the old angles
        oldServo0Angle = servo0Angle;
        oldServo1Angle = servo1Angle;
      }
    }
  }
}

void setServoPosition(int channel, int angle) {
  int pulse = map(angle, 30, 150, servoMin, servoMax);
  servoShield.setPWM(channel, 0, pulse);
}
