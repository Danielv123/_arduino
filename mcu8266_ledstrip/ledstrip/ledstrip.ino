#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "IOT";
const char* password = "aq12wsxc";
String hostname = "RGBStrip-";
const int ledPin = LED_BUILTIN; // Or D0;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  hostname += String(ESP.getChipId(), HEX);
  ArduinoOTA.setHostname(hostname.c_str());
  Serial.println("Set hostname");
  Serial.println(hostname);

  // Authentication (optional)
  ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT);
}

long previousMillis = 0;
const long interval = 1000;

// Make sure to only put non-blocking code in event loop
void loop() {
  ArduinoOTA.handle();

  long currentMillis = millis();

  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;

    int ledState = digitalRead(ledPin);
    // digitalWrite(ledPin, !ledState); // Active low
  }
  analogWrite(ledPin, abs(currentMillis - previousMillis - 500)-100);
}
