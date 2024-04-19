#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>  // Include the ArduinoJson library for JSON handling

const char* ssid = "AnasAlsayed-2.4";
const char* password = "1234567890";
const char* serverUrl = "http://192.168.1.14:5000/sensor_data_post";
const char* commandEndpoint = "http://192.168.1.14:5000/command";

WiFiClient wifiClient;
HTTPClient http;

void setup() {
  Serial.begin(9600); // Start the serial communication for debugging
  WiFi.begin(ssid, password); // Initiate WiFi connection with the provided SSID and password
  
  while (WiFi.status() != WL_CONNECTED) { // Wait until the connection is established
    delay(500);
    Serial.print("."); // Print dots on the Serial monitor to indicate waiting
  }
  Serial.println("WiFi connected"); // Print to Serial once connected
  Serial.print("IP Address: "); 
  Serial.println(WiFi.localIP()); // Print the local IP address assigned to the ESP
}

void loop() {
  if (Serial.available()) { // Check if there is data sent from the Serial monitor
    String sensorData = Serial.readStringUntil('\n'); // Read the sensor data until newline
    postData(sensorData); // Post the data to the server
  }
  
  // Check for command from server every 5 seconds
  static unsigned long lastCheckMillis = millis();
  if (millis() - lastCheckMillis > 5000) {
    lastCheckMillis = millis();
    checkForCommand();
  }
}

void postData(String sensorData) {
  http.begin(wifiClient, serverUrl); // Begin a HTTP client connection to the server URL
  http.addHeader("Content-Type", "application/json"); // Set the content type of the request to JSON
  String jsonPayload = createJsonFromSensors(sensorData); // Create JSON payload from sensor data
  int httpResponseCode = http.POST(jsonPayload); // Send the data as a POST request
  Serial.println("HTTP Response Code: " + String(httpResponseCode)); // Print the HTTP response code to Serial
  http.end(); // End the HTTP client connection
}

void checkForCommand() {
  http.begin(wifiClient, commandEndpoint); // Begin a HTTP client connection to the command URL
  int httpResponseCode = http.GET(); // Send a GET request
  
  if (httpResponseCode == 200) {
    String payload = http.getString(); // Get the response payload
    Serial.println("Command received: " + payload); // Print the received command
  } else {
    Serial.println("Failed to receive command, HTTP code: " + String(httpResponseCode)); // Print error if not successful
  }
  http.end(); // End the HTTP client connection
}

String createJsonFromSensors(String sensorData) {
  DynamicJsonDocument doc(1024);
  int startIndex = 0, endIndex = 0;
  String keys[] = {"water_level", "ph_value", "voltage", "current", "motion_detected", "water_temperature"};
  float values[6];
  int index = 0;
  while (endIndex != -1 && index < 6) {
    endIndex = sensorData.indexOf(',', startIndex);
    String value = sensorData.substring(startIndex, endIndex == -1 ? sensorData.length() : endIndex);
    values[index] = value.toFloat(); // Convert string to float and store temporarily
    startIndex = endIndex + 1;
    index++;
  }

  // Check for invalid temperature value
  if (values[5] < 0) {  // Assuming index 5 is temperature
    values[5] = 0; // Set a default or corrected value
    Serial.println("Invalid temperature detected, setting to default 0");
  }

  // Assign values to JSON document
  for (index = 0; index < 6; index++) {
    doc[keys[index]] = values[index];
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println("Sending JSON: " + jsonString); // Print JSON to Serial for debugging
  return jsonString;
}
