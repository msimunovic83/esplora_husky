#include <Esplora.h>
#include <SoftwareSerial.h>
#include "HUSKYLENS.h"

#define HUSKYLENS_RX 14 // ICSP MISO (Pin 1)
#define HUSKYLENS_TX 16 // ICSP MOSI (Pin 4)
#define ID1 1

class HUSKYLENS_SoftwareSerial : public HUSKYLENS {
  private:
    SoftwareSerial *softSerial;

  public:
    HUSKYLENS_SoftwareSerial(SoftwareSerial &serial) : HUSKYLENS() {
      softSerial = &serial;
    }

    bool begin() {
      if (!softSerial) return false;
      softSerial->begin(9600);
      return HUSKYLENS::begin(*softSerial);
    }
};

SoftwareSerial mySerial(HUSKYLENS_RX, HUSKYLENS_TX);
HUSKYLENS_SoftwareSerial huskylens(mySerial);

// Available algorithms
const byte algorithms[] = {
   ALGORITHM_FACE_RECOGNITION,
    ALGORITHM_OBJECT_TRACKING,
    ALGORITHM_OBJECT_RECOGNITION,
    ALGORITHM_LINE_TRACKING,
    ALGORITHM_COLOR_RECOGNITION,
    ALGORITHM_TAG_RECOGNITION,
    ALGORITHM_OBJECT_CLASSIFICATION
};
const byte numAlgorithms = sizeof(algorithms) / sizeof(algorithms[0]);
byte currentAlgorithmIndex = 0;

void setup() {
  Serial.begin(9600); // Initialize the Serial Monitor

  while (!huskylens.begin()) {
    Esplora.writeRGB(255, 0, 0); // Turn the Esplora's RGB LED red
    delay(100);
    Esplora.writeRGB(0, 0, 0); // Turn off the Esplora's RGB LED
    delay(100);
  }
}

void loop() {
  static int id1Counter = 0;
  bool id1Detected = false;

 int x = Esplora.readJoystickX();

  if (x > 300) {
    // Move to the next algorithm
    currentAlgorithmIndex = (currentAlgorithmIndex + 1) % numAlgorithms;
    huskylens.writeAlgorithm(algorithms[currentAlgorithmIndex]);
    delay(500);
  } else if (x < -300) {
    // Move to the previous algorithm
    currentAlgorithmIndex = (currentAlgorithmIndex - 1 + numAlgorithms) % numAlgorithms;
    huskylens.writeAlgorithm(algorithms[currentAlgorithmIndex]);
    delay(500);
  }






  if (huskylens.request()) {
    for (int i = 0; i < huskylens.available(); i++) {
      HUSKYLENSResult result = huskylens.read();
      if (result.ID == ID1) {
        id1Detected = true;
        break;
      }
    }

    if (id1Detected) {
      id1Counter++; // Increment the counter for object ID1
      Serial.print("Object ID1 was shown ");
      Serial.print(id1Counter);
      Serial.println(" times.");

      int potValue = Esplora.readSlider(); // Read the value of the linear potentiometer
      int frequency = map(potValue, 0, 1023, 100, 4000); // Map the potentiometer value to a frequency range

      Esplora.tone(frequency, 500); // Play the tone with the mapped frequency for 500 ms
      delay(500);
      Esplora.noTone(); // Stop the tone
    }
  }

      // Check if Switch 1 is pressed
  if (Esplora.readButton(SWITCH_1) == LOW) {
    // Send the command to learn a new object as ID1
    huskylens.writeAlgorithm(ALGORITHM_FACE_RECOGNITION);
    huskylens.writeLearn(ID1);
  }

      // Check if Switch 2 is pressed
  if (Esplora.readButton(SWITCH_2) == LOW) {
    // Send the command to stop learning by switching to the default object tracking algorithm
    huskylens.writeAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION);
  }

      // Check if Switch 4 is pressed
  if (Esplora.readButton(SWITCH_3) == LOW) {
    // Send the command to stop learning by switching to the default object tracking algorithm
    huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
  }

        // Check if Switch 4 is pressed
  if (Esplora.readButton(SWITCH_4) == LOW) {
    // Send the command to stop learning by switching to the default object tracking algorithm
    huskylens.writeAlgorithm(ALGORITHM_TAG_RECOGNITION);
  }

  delay(50);
}
