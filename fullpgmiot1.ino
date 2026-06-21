#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust address if needed

// Sensor pins
const int trigPin1 = 9;
const int echoPin1 = 10;
const int trigPin2 = 11;
const int echoPin2 = 12;

// Buzzer pin
const int buzzerPin = 8;

// Distance between sensors (cm) — update with your actual spacing!
const float sensorDistance = 5.0; // example: 5 cm

// Variables
unsigned long startTime = 0;
unsigned long endTime = 0;
bool sensor1Triggered = false;
bool sensor2Triggered = false;

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30 ms timeout
  return duration * 0.034 / 2; // cm
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(buzzerPin, LOW); // buzzer OFF at start

  lcd.setCursor(0,0);
  lcd.print("Speed Detection");
  delay(2000);
}

void beepThreeTimes() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(400);
    digitalWrite(buzzerPin, LOW);
    delay(400);
  }
}

void loop() {
  int distance1 = getDistance(trigPin1, echoPin1);
  int distance2 = getDistance(trigPin2, echoPin2);

  if (distance1 > 0 && distance1 < 30 && !sensor1Triggered) {
    sensor1Triggered = true;
    startTime = millis();
    Serial.println("Sensor 1 triggered");
  }

  if (distance2 > 0 && distance2 < 30 && sensor1Triggered && !sensor2Triggered) {
    sensor2Triggered = true;
    endTime = millis();
    Serial.println("Sensor 2 triggered");

    float timeTaken = (endTime - startTime) / 1000.0; // seconds
    float distanceMeters = sensorDistance / 100.0;    // cm → m
    float speed = (distanceMeters / timeTaken) * 3.6; // km/h

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Speed:");
    lcd.print(speed,1);
    lcd.print(" km/h");

    Serial.print("Speed: "); Serial.println(speed);

    if (speed > 5.0) {
      lcd.setCursor(0,1);
      lcd.print("Overspeed!");
      beepThreeTimes();
    } else {
      lcd.setCursor(0,1);
      lcd.print("Safe Speed");
      digitalWrite(buzzerPin, LOW);
    }

    sensor1Triggered = false;
    sensor2Triggered = false;
    delay(2000);
    digitalWrite(buzzerPin, LOW);
  }
}
