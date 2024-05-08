#include <SoftwareSerial.h>

#define SIM868_TX 2              // Connect SIM868 TX to pin 2 of Arduino Uno
#define SIM868_RX 3              // Connect SIM868 RX to pin 3 of Arduino Uno
#define VIBRATION_SENSOR_PIN 4   // Connect the DO pin of the vibration sensor to pin 4 of Arduino Uno
#define LDR_SENSOR_PIN A0        // Connect LDR sensor to Analog Pin A0

#define TRIGGER_PIN 9
#define ECHO_PIN 10
#define LED_PIN 11

SoftwareSerial sim868Serial(SIM868_RX, SIM868_TX);

const String phoneNumber1 = "+919498895906";
const String phoneNumber2 = "+919578887900";
const String phoneNumber3 = "+919150685596";


unsigned long lastVibrationTime = 0;
unsigned long lastLDRCheckTime = 0;
const unsigned long vibrationInterval = 30000;
const unsigned long ldrCheckInterval = 30000;

long duration;
int distance;
int ledBrightness = 100;

void setup() {
  Serial.begin(9600);
  sim868Serial.begin(9600);
  pinMode(VIBRATION_SENSOR_PIN, INPUT);
  pinMode(LDR_SENSOR_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  delay(1000);
  sim868Serial.println("AT+CGNSPWR=1");
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  // Vibration Sensor Section
  if (currentMillis - lastVibrationTime >= vibrationInterval) {
    lastVibrationTime = currentMillis;
    Serial.println("Checking vibration...");
    int vibrationSensorValue = digitalRead(VIBRATION_SENSOR_PIN);
    Serial.print("Vibration sensor value: ");
    Serial.println(vibrationSensorValue);
    if (vibrationSensorValue == HIGH) {
      Serial.println("Vibration detected. Sending notification...");
      sendCrashNotification();
    } else {
      Serial.println("No vibration detected.");
    }
  }

  // LDR Sensor Section
  if (currentMillis - lastLDRCheckTime >= ldrCheckInterval) {
    lastLDRCheckTime = currentMillis;
    Serial.println("Checking LED status...");
    int ldrValue = analogRead(LDR_SENSOR_PIN);
    Serial.print("LDR sensor value: ");
    Serial.println(ldrValue);
    if (ldrValue < 100) {
      Serial.println("LED status OK.");
    } else {
      Serial.println("LED fault detected. Sending notification...");
      sendFaultNotification();
    }
  }

  // Ultrasonic Sensor Section
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < 20) {
    for (int i = 0; i < 5; i++) {
      analogWrite(LED_PIN, 255);
      delay(1000);
    }
    analogWrite(LED_PIN, ledBrightness);
  } else {
    analogWrite(LED_PIN, ledBrightness);
  }

  // Serial.print("Distance: ");
  // Serial.print(distance);
  // Serial.println(" cm");

  delay(500);
}

void sendCrashNotification() {
  String latitude = "11.015146399101134";
  String longitude = "76.95661439261768";
  String googleMapsLink = "https://maps.app.goo.gl/vUt52aMzkSXSjaxQA";
  String smsMessage = "Street Light Crash Detected \nLocation: " + latitude + ", " + longitude + ". \nGoogle Maps Link: " + googleMapsLink;
  sendSMS(phoneNumber1, smsMessage);
  sendSMS(phoneNumber2, smsMessage);
  sendSMS(phoneNumber3, smsMessage);
}

void sendFaultNotification() {
  String faultMessage = "Light Fault Detected. Please fix it!";
  sendSMS(phoneNumber1, faultMessage);
  sendSMS(phoneNumber2, faultMessage);
  sendSMS(phoneNumber3, faultMessage);
}

void sendSMS(String number, String message) {
  sim868Serial.println("AT+CMGF=1");
  delay(1000);
  sim868Serial.println("AT+CMGS=\"" + number + "\"");
  delay(1000);
  sim868Serial.print(message);
  delay(100);
  sim868Serial.write(26);
  delay(5000);

  while (sim868Serial.available()) {
    Serial.write(sim868Serial.read());
  }
  Serial.println();
}
