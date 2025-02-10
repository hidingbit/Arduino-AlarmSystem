#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial arduinoSerial(9, 10); // RX, TX

const int calibrationTime = 5000;
const int infoUpdateInterval = 15000;
unsigned long lastCalibrationTime = 0;
unsigned long lastInfoUpdateTime = 0;

const int greenPin = 4;
const int redPin = 5;
const int buttonPin = 6;
const int flamePin = 7;
const int laserPin = 3;
const int photoResistorPin = A4;
const int temperaturePin = A5;
const int piezoPin = 2;

int threshold = 0;
int photoResistorValue = 0;
int temperatureValue = 0;
int flameValue = 0;
int buttonState = 0;

bool intrusionDetected = false;
bool fireDetected = false;

void setup() {
  Serial.begin(115200);
  arduinoSerial.begin(9600);

  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(flamePin, INPUT);
  pinMode(laserPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(laserPin, HIGH);

  calibration();
}

void calibration() {
  buttonState = LOW;
  threshold = 0;
  // Start calibration
  Serial.print("Calibrating...");
  lastCalibrationTime = millis();
  while(millis() - lastCalibrationTime < calibrationTime) {
    Serial.print(".");
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    delay(500);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    photoResistorValue = analogRead(photoResistorPin);
    delay(500);
    if (photoResistorValue > threshold) {
      threshold = photoResistorValue - 150;
    }
  }
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  Serial.println("Calibration completed.");
}

void loop() {
  photoResistorValue = analogRead(photoResistorPin);
  delay(10);
  temperatureValue = analogRead(temperaturePin);
  delay(10);
  flameValue = digitalRead(flamePin);
  delay(10);

  float voltage = (temperatureValue/1024.0) * 5.0;
  float temperature = (voltage - .5) * 100;

  Serial.println("Photoresistor: " + String(photoResistorValue) + "  |  Threshold: " + String(threshold) + "  |  Temperature: " + String(temperature) + " °C" + "  |  Flame: " + String(flameValue));
  
  if (photoResistorValue < threshold && !intrusionDetected) { //Intrusion detected
    intrusionDetected = true;
    Serial.println("Intrusion detected");
    emitEvent("intrusion", temperature);
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
  }
  
  if (flameValue == 1 && !fireDetected) { // Fire detected
    fireDetected = true;
    Serial.println("Fire detected");
    emitEvent("fire", temperature);
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
  }

  if (millis() - lastInfoUpdateTime > infoUpdateInterval) { // Send "heartbeat" message about current status every infoUpdateInterval
    Serial.println("Sending heartbeat message...");
    emitEvent("info", temperature);
    lastInfoUpdateTime = millis();
  }

  if (intrusionDetected || fireDetected) { // Make the buzzer sound like an alarm only if an intrusion/fire is detected
      for (int freq = 500; freq <= 1000; freq += 50) {
        tone(piezoPin, freq); 
        delay(10);             
      }
      for (int freq = 1000; freq >= 500; freq -= 50) {
        tone(piezoPin, freq);
        delay(10);             
      }
  }

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) { // Stop the alarm sound and reset the system
    intrusionDetected = false;
    fireDetected = false;
    noTone(piezoPin);
    calibration();
  };
}

void emitEvent(char * eventType, float temperature) { // Sending messages via UART
  JsonDocument doc;
  doc["type"] = eventType;
  doc["temperature"] = String(temperature) + " °C";
  String data;
  serializeJson(doc, data);
  arduinoSerial.println(data);
}
