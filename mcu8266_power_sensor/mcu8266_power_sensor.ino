#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

char mqtt_server[40];
#define mqtt_server       "iot.eclipse.org"
#define mqtt_port         "1883"
#define mqtt_user         "mqtt_user"
#define mqtt_pass         "mqtt_pass"
#define mqtt_topic        "Danielv123/power"
#define device_name       "Livingroom power monitor"

WiFiClient espClient;
PubSubClient client(espClient);


//for LED status
#include <Ticker.h>
Ticker ticker;
void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}
extern "C" {
#include "user_interface.h"
}

#define SUM_MAX 600
#define ADC_MAX 1024

int count = 0;
int adc_avr = 0;
uint16_t vals[ADC_MAX+1];
unsigned long time_m;
unsigned long time_mn;
float Ieff;
float P;
float W;

void setup() {

  // initialize serial communication:
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 20);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 20);
  WiFiManagerParameter custom_mqtt_topic("topic", "mqtt topic", mqtt_topic, 20);
  WiFiManagerParameter custom_device_name("device_name", "device name", device_name, 40);
  
  WiFiManager wifiManager;
  
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_mqtt_topic);
  wifiManager.addParameter(&custom_device_name);
  
  wifiManager.setAPCallback(configModeCallback);
  //reset saved settings
  //wifiManager.resetSettings();

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  Serial.println("Connected to WIFI");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  
  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_pass, custom_mqtt_pass.getValue());
  strcpy(mqtt_topic, custom_mqtt_topic.getValue());
  strcpy(device_name, custom_device_name.getValue());
  
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);
  delay(500);

  // atoi() converts the char* mqtt_port to an int
  client.setServer(mqtt_server, atoi(mqtt_port));
  
  time_m = millis();
}

void loop() {
  delay(1000);
  if (!client.connected())  {
    reconnect();
  }
  do_measurements(50);
  client.loop();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
    //if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void do_measurements(int n) {
  uint16_t last_adc_read;
  
  for (;n > 0; n--) {
    count++;
    if (count >= SUM_MAX) {
      float U, Ueff;
      float Sum_U_Square= 0.0;
  
      count = 0;
      adc_avr = adc_avr/SUM_MAX;
  
      for (int i = 0; i < ADC_MAX+1; i++) {
        if (vals[i] != 0) {
          //Serial.print(i);Serial.print(": ");
          //Serial.print(vals[i]);Serial.println(" ");

          if (abs(i-adc_avr) > 2) {
            // 1024 is the max input value, 5000 is 5v * the transformer being 1:1000
            U = ((float)(i - adc_avr))/1024 * 3.3 * 1000;
            Sum_U_Square += U*U*vals[i]; 
          }
          vals[i] = 0;
        }
      }
      Ueff = sqrt(Sum_U_Square/SUM_MAX);
      Ieff = Ueff/185.0;
      P = Ieff*230;
      time_mn = millis();
      W += P * (time_mn - time_m)/3600000.0;
      time_m = time_mn;

      Serial.print(String(adc_avr)+"  ");
      Serial.print(String(Ueff)+"mV  ");
      Serial.print(String(Ieff)+"A  ");
      Serial.print(String(P)+"W ");
      Serial.println(String(W)+"Wh");

      // Send data to MQTT
      String payload = String(device_name)+"§"+String(adc_avr)+String(Ueff)+"|mV§"+String(Ieff)+"|A§"+String(P)+"|W§"+String(W)+"|Wh§";
      char msg[payload.length()+1];
      payload.toCharArray(msg, sizeof(msg));
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(mqtt_topic, msg);
      adc_avr = 0;
    }

    last_adc_read = analogRead(A0);
    if (last_adc_read > ADC_MAX) {
      Serial.println("ADC error");
      continue;
    }
    vals[last_adc_read]++; 
    adc_avr += last_adc_read;
    delay(2);
  }
}
