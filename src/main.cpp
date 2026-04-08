#include <WiFi.h>
#include <WiFiClientSecure.h>  
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// WiFi
const char* ssid = "solow";
const char* password = "@SoLow001";

//Server
const char* serverName = "https://end-to-end-iot-smart-agriculture-system-with-rea-production.up.railway.app/data";

// DHT
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensors
#define MQ135_PIN 34
#define SOIL_PIN 35

// Timing
unsigned long lastTime = 0;
unsigned long timerDelay = 10000; 

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Read Sensors
void readSensors(float &temp, float &hum, int &air, int &soil) {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  air = analogRead(MQ135_PIN);
  soil = analogRead(SOIL_PIN);
}

// Send Data
void sendData(float temp, float hum, int air, int soil) {
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();   

    HTTPClient http;
    http.begin(client, serverName); 

    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["temperature"] = temp;
    doc["humidity"] = hum;
    doc["air_quality"] = air;
    doc["soil_moisture"] = soil;

    String jsonString;
    serializeJson(doc, jsonString);

    int response = http.POST(jsonString);

    Serial.print("HTTP Response: ");
    Serial.println(response);

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Loop
void loop() {
  if ((millis() - lastTime) > timerDelay) {

    float temp, hum;
    int air, soil;

    readSensors(temp, hum, air, soil);

    Serial.println("Sending Data...");
    Serial.print("Temp: "); Serial.println(temp);
    Serial.print("Humidity: "); Serial.println(hum);
    Serial.print("Air: "); Serial.println(air);
    Serial.print("Soil: "); Serial.println(soil);

    sendData(temp, hum, air, soil);

    lastTime = millis();
  }
}