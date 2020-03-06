// Some necessary libraries
#include <SD.h>
#include <TMRpcm.h>
#include <Wire.h>

// Initiate some instances
TMRpcm music;
const byte slaveAddress = 9;
const byte answerSize = sizeof(int);
const byte ledPin = 30;
const byte blinkerIn = 2;
unsigned long prevFlash;

byte receiveBuffer[2];
int distance;
int prevDist;
const byte tonePin = 11;





void setup() {
  
  Serial.begin(9600);                 // Begin Serial Communication for Debugging  
  music.speakerPin = tonePin;         // Configure the Speaker Out Pin
  pinMode(ledPin, OUTPUT);            // An LED is used to notify the presence of an obstacle
  pinMode(blinkerIn, INPUT);          // The blinkerIn Pin reads signal from the turn signal wire in the vehicle
  if (!SD.begin(53)) {                // Starts the SD module, return if there is an error
    Serial.println("SD problem");
    return;
  }

  Wire.begin();
  
  Serial.println("I2C master from the mega");
  attachInterrupt(digitalPinToInterrupt(blinkerIn), blinkerISR, FALLING); // This interrupt takes the inpulse signal from the 
                                                                          // turn signal, and the rest of the code will decide
                                                                          // if the turn signal is on


}

void loop() {
  Wire.requestFrom(slaveAddress, answerSize);   // Request distance information from the nano

// Takes the distance as integer (2 bytes)
  if (2 <= Wire.available()){
    receivedInt = Wire.read();
    receivedInt = receivedInt << 8;
    receivedInt |= Wire.read();
  }
  
  distance = receivedInt;
  Serial.println(receivedInt);
  
// If the distance is less than 20, the distance measurement is probably out of range.
// I used a zero order hold method, which means that the current value will just be
// the previous value.
  if (distance >= 20) {
    prevDist = distance;
  }

// If there is an obstacle, i.e. distance less than 2 meters
  if (prevDist < 200) {
 

    digitalWrite(ledPin, HIGH);

// If the turn signal is on, i.e. the time interval between two flashed is less than a second,
// Then sound the alarm
    if ((millis() - prevFlash) < 1000) {
      tone(tonePin, 700);
      delay(200);
      noTone(tonePin);
      delay(100);
// The commented out code can be used to play something else from the SD card
      //music.play("2.wav");
      //delay(1300  );
    }
  }


// If there is no obstacle
  else {
    digitalWrite(ledPin, LOW);
  }

  //Serial.println(digitalRead(blinkerIn));
  


}


// The interrupt service routine, which updates the flash time
void blinkerISR(){
  prevFlash = millis();
}
