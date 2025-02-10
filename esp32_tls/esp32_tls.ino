#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <arduino_secrets.h>

#define RXD2 16
#define TXD2 17

HardwareSerial espSerial(2);

const char ssid[] = SECRET_SSID;
const char password[] = SECRET_PASSWORD;

const char mqtt_broker_address[] = SECRET_MQTT_BROKER_ADDRESS;
const int mqtt_port = SECRET_MQTT_PORT;
const char mqtt_client_id[] = SECRET_MQTT_CLIENT_ID;

const char ca_cert[] = SECRET_CA_CERT;
const char client_cert[] = SECRET_CLIENT_CERT;
const char client_key[] = SECRET_CLIENT_KEY;

// The MQTT topics that ESP32 should publish/subscribe
const char INTRUSION_TOPIC[] = "intrusion";
const char FIRE_TOPIC[] = "fire";
const char INFO_TOPIC[] = "info";

WiFiClientSecure wifiClient;
MQTTClient mqtt = MQTTClient(256);

unsigned long lastPublishTime = 0;

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  
  WiFi.softAP(ssid, password);
  Serial.println("Turning on Access Point...");
  IPAddress IP = WiFi.softAPIP();

  Serial.print("Access Point ready! IP address is: ");
  Serial.println(IP);

  wifiClient.setCACert(ca_cert);
  wifiClient.setCertificate(client_cert);
  wifiClient.setPrivateKey(client_key);
  
  connectToMQTT();
}

void loop() {
  mqtt.loop();
  while (espSerial.available() > 0) {
    String received = espSerial.readStringUntil('\n');
    Serial.print("Arduino message: ");
    Serial.println(received);
    sendToMQTT(received);
  }
}

void connectToMQTT() {
  mqtt.begin(mqtt_broker_address, mqtt_port, wifiClient);

  Serial.print("ESP32 - Connecting to MQTT broker...");
  
  while (!mqtt.connect(mqtt_client_id)) {
    Serial.print(".");
    delay(100);
  }

  if (!mqtt.connected()) {
    Serial.println("ESP32 - MQTT broker Timeout!");
    return;
  }

  Serial.println("\nConnected to the broker successfully!");
}

void sendToMQTT(String arduinoMessage) {
  if (!mqtt.connected()) {
    Serial.println("ESP32 - MQTT broker Timeout!");
    connectToMQTT();
  }
  JsonDocument doc;
  deserializeJson(doc, arduinoMessage);
  doc["client_id"] = mqtt_client_id;
  String message;
  serializeJson(doc, message);
  
  const char* type = doc["type"];
  if (strcmp(type, INTRUSION_TOPIC) == 0 || strcmp(type, FIRE_TOPIC) == 0 || strcmp(type, INFO_TOPIC) == 0) {
    mqtt.publish(type, message);
    Serial.println("ESP32 - A message has been sent via MQTT:");
    Serial.print("- topic: ");
    Serial.println(type);
    Serial.print("- payload: ");
    Serial.println(message);
  } else Serial.println("Unrecognized topic.");
}
