#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// WiFi credentials
char ssid[] = "AnasAlsayed-2.4";
char pass[] = "1234567890";

// Blynk Auth Token
char auth[] = "inKmhaalmO4jR3Eh5pkJucMisKI37N1o";

// Define ESP8266 Serial for WiFi module
#define EspSerial Serial1
#define ESP8266_BAUD 9600
ESP8266 wifi(&EspSerial);

// Sensor and relay pins
const int pHsensorPin = A1;       // Analog input pin for pH sensor
const int chlorinePumpPin = 22;   // Digital pin to control the chlorine pump relay
const int motionSensorPin = 4;    // Digital pin connected to the motion sensor

// Threshold values
float chlorineThreshold = 7.5;    // pH threshold for activating the chlorine pump
float normalVoltageThreshold = 5.0; // Normal voltage threshold
float normalCurrentThreshold = 1.0; // Normal current threshold

// Flags for manual control
bool isChlorinePumpManual = false; // Flag for manual control of the chlorine pump

BlynkTimer timer;

// Function prototypes
float readPHSensor();
void checkPHAndAdjustChlorine();
void sendSensorData();
void motionDetection();

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  // Initialize Blynk
  Blynk.begin(auth, wifi, ssid, pass);

  // Setup sensor and actuator pins
  pinMode(pHsensorPin, INPUT);
  pinMode(chlorinePumpPin, OUTPUT);
  pinMode(motionSensorPin, INPUT);

  // Timers
  timer.setInterval(10000L, checkPHAndAdjustChlorine); // Adjust timing as needed
  timer.setInterval(500L, motionDetection); // Adjust timing as needed
  timer.setInterval(1000L, sendSensorData); // Adjust the interval as needed
}

void loop() {
  Blynk.run();
  timer.run();
}

float readPHSensor() {
  int sensorValue = analogRead(pHsensorPin);
  float pH = (float)sensorValue / 1024 * 14; // Example conversion
  return pH;
}

void checkPHAndAdjustChlorine() {
  if (!isChlorinePumpManual) {
    float pHValue = readPHSensor();
    Blynk.virtualWrite(V1, pHValue); // Send pH value to Blynk app

    if (pHValue > chlorineThreshold) {
      digitalWrite(chlorinePumpPin, HIGH); // Activate chlorine pump
    } else {
      digitalWrite(chlorinePumpPin, LOW); // Deactivate chlorine pump
    }
  }
}

BLYNK_WRITE(V2) { // Virtual pin for manual pump control
  isChlorinePumpManual = param.asInt();
  digitalWrite(chlorinePumpPin, isChlorinePumpManual);
}

void sendSensorData() {
  // Placeholder for actual voltage and current reading logic
  float voltage = analogRead(A2) * (5.0 / 1023.0); // Example conversion
  float current = analogRead(A3) * (5.0 / 1023.0); // Example conversion

  if (voltage < normalVoltageThreshold || current < normalCurrentThreshold) {
    Blynk.notify("Alert: Solar generation is below normal!");
  }

  Blynk.virtualWrite(V3, voltage);
  Blynk.virtualWrite(V4, current);
}

void motionDetection() {
  if (digitalRead(motionSensorPin) == HIGH) {
    Blynk.notify("Motion detected in the pool area!");
  }
}
