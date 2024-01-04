#include <SoftwareSerial.h>

SoftwareSerial EspSerial(18, 19); // RX, TX

// Sensor and relay pins
const int waterLevelSensorPin = A0;  // Analog input pin for water level sensor
const int pHsensorPin = A1;          // Analog input pin for pH sensor
const int voltageSensorPin = A2;     // Analog input pin for voltage sensor
const int currentSensorPin = A3;     // Analog input pin for current sensor
const int motionSensorPin = 4;       // Digital pin connected to the motion sensor
const int chlorinePumpPin = 22;      // Digital pin to control the chlorine pump relay

void setup() {
  EspSerial.begin(9600); // Start serial communication with ESP8266
  pinMode(waterLevelSensorPin, INPUT);
  pinMode(pHsensorPin, INPUT);
  pinMode(voltageSensorPin, INPUT);
  pinMode(currentSensorPin, INPUT);
  pinMode(motionSensorPin, INPUT);
  pinMode(chlorinePumpPin, OUTPUT);
}

void loop() {
  int waterLevel = analogRead(waterLevelSensorPin);
  int pHValue = analogRead(pHsensorPin);
  int voltageValue = analogRead(voltageSensorPin);
  int currentValue = analogRead(currentSensorPin);
  int motionValue = digitalRead(motionSensorPin);

  // Send data to ESP8266
  EspSerial.print(waterLevel);
  EspSerial.print(",");
  EspSerial.print(pHValue);
  EspSerial.print(",");
  EspSerial.print(voltageValue);
  EspSerial.print(",");
  EspSerial.print(currentValue);
  EspSerial.print(",");
  EspSerial.println(motionValue);

  delay(1000); // Delay in between reads for stability
}
