#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "YourWiFiSSID";        // WiFi SSID
char pass[] = "YourWiFiPassword";    // WiFi Password
char auth[] = "YourBlynkAuthToken";  // Blynk Auth Token

BlynkTimer timer;

void setup() {
  Serial.begin(9600); // Start serial communication with Arduino
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendDataToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendDataToBlynk() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);
    int thirdComma = data.indexOf(',', secondComma + 1);
    int fourthComma = data.indexOf(',', thirdComma + 1);

    int waterLevel = data.substring(0, firstComma).toInt();
    float pH = data.substring(firstComma + 1, secondComma).toFloat() / 1024.0 * 14.0; // Example conversion
    float voltage = data.substring(secondComma + 1, thirdComma).toFloat() * (5.0 / 1023.0); // Example conversion
    float current = data.substring(thirdComma + 1, fourthComma).toFloat() * (5.0 / 1023.0); // Example conversion
    int motion = data.substring(fourthComma + 1).toInt();

    Blynk.virtualWrite(V1, waterLevel);
    Blynk.virtualWrite(V2, pH);
    Blynk.virtualWrite(V3, voltage);
    Blynk.virtualWrite(V4, current);

    if (motion == HIGH) {
      Blynk.notify("Motion detected!");
    }
  }
}
