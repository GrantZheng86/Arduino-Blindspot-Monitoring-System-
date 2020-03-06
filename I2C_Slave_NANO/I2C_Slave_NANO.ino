// Some necessary libraries
#include <Wire.h>
#include <Math.h>
#include <MedianFilter.h>

// Some constants
const byte trigPin = 7;
const byte echoPin = 8;
const byte slaveAddress = 9;
const byte answerSize = sizeof(int);

// Some instances
MedianFilter filter(5, 0);  // The median filter will take the median of 5 consecutive values from the sensor reading
long duration;
int distance;
int prevDist;
int filteredDist;
int finalDist;
byte resetMega = 12;


void setup() {
  Serial.begin(9600);           // Begin Serial communication for debugging
  Wire.begin(slaveAddress);     // Begin I2C communication, as a slave, with the mega
  Wire.onRequest(requestEvent); // Things to do when on request
  pinMode(trigPin, OUTPUT);     // The trig pin connected to the radar is the output pin
  pinMode(echoPin, INPUT);      // Echo is the input
  // Resets the mega for more stable communication
  pinMode(resetMega, OUTPUT);
  delay(1000);
  digitalWrite(resetMega, LOW);
  delay(200);
  digitalWrite(resetMega, HIGH);

}

void loop() {

  // Send trig singal to the radar
  digitalWrite(trigPin, LOW);
  delay(5);
  digitalWrite(trigPin, HIGH);
  delay(11);
  digitalWrite(trigPin, LOW);

  // Reads duration of echo pulse
  duration = pulseIn(echoPin, HIGH, 36000);


  // If the duration is 0, this means that the distance
  // is out of range, hold the previous value as the current one.
  if (duration == 0){
    distance = prevDist;
  } else {
    // the distance is the duration divided by the speed of sound
    distance = duration * 0.034/2.0;
  }
  prevDist = distance;

  // Median filter
  filter.in(prevDist);
  filteredDist = filter.out();

  // Low pass filter
  finalDist = round((float)finalDist * 0.1777 + (float)filteredDist * 0.8233);
  
  
  
  Serial.println(finalDist);
}


void requestEvent() {
  
  byte transferBuffer[2];

  // This breaks down the integer as two bytes to transfer over the I2C bus
  transferBuffer[1] = byte(finalDist & 0xFF);
  transferBuffer[0] = byte((finalDist >> 8) & 0XFF);
  Wire.write(transferBuffer, sizeof(int));
  
}
