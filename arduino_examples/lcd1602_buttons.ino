/*
  LCD1602 + Buttons Example
  - Uses LiquidCrystal library (included with Arduino IDE)
  - Connect LCD as follows (typical wiring):
    RS -> D12
    E  -> D11
    D4 -> D5
    D5 -> D4
    D6 -> D3
    D7 -> D2
    VSS -> GND
    VDD -> 5V
    V0  -> Potentiometer (contrast)
    RW  -> GND
    A   -> 5V (backlight)
    K   -> GND (backlight)
  - Button1: Increment (D8, active LOW)
  - Button2: Decrement (D9, active LOW)
*/
#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 7      // DHT data pin connected to D7
#define DHTTYPE DHT11 // or DHT22
#define BUZZER_PIN 6  // Buzzer connected to D6

#define TRIG_PIN 8    // HC-SR04 Trig pin
#define ECHO_PIN 9    // HC-SR04 Echo pin

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
DHT dht(DHTPIN, DHTTYPE);

// Non-blocking timing variables
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000; // ms

unsigned long buzzerSirenStart = 0;
bool buzzerSirenActive = false;
const int buzzerSirenCycles = 10;
const unsigned long buzzerOnTime = 50;  // ms
const unsigned long buzzerOffTime = 50; // ms

float lastHumidity = NAN;
float lastDistance = NAN;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout (~5m)
  if (duration == 0) return NAN;
  return duration * 0.0343 / 2.0;
}

void loop() {
  unsigned long now = millis();

  // Update readings and display at interval
  if (now - lastUpdate >= updateInterval) {
    lastHumidity = dht.readHumidity();
    lastDistance = readDistanceCM();

    lcd.setCursor(0, 0);
    lcd.print("H:");
    if (isnan(lastHumidity)) {
      lcd.print("Err   ");
    } else {
      lcd.print(lastHumidity, 1);
      lcd.print("%   ");
    }

    lcd.setCursor(0, 1);
    lcd.print("D:");
    if (isnan(lastDistance)) {
      lcd.print("Err   ");
    } else {
      lcd.print(lastDistance, 1);
      lcd.print("cm   ");
    }

    // Buzzer logic for humidity
    if (!isnan(lastHumidity) && lastHumidity < 60 && !buzzerSirenActive) {
      buzzerSirenActive = true;
      buzzerSirenStart = now;
      digitalWrite(BUZZER_PIN, HIGH); // Start with buzzer ON
    } else if (!isnan(lastHumidity) && lastHumidity >= 60) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerSirenActive = false;
    }
    lastUpdate = now;
  }

  // Non-blocking buzzer siren
  if (buzzerSirenActive) {
    unsigned long sirenElapsed = millis() - buzzerSirenStart;
    unsigned long cycleTime = buzzerOnTime + buzzerOffTime;
    int currentCycle = sirenElapsed / cycleTime;
    if (currentCycle < buzzerSirenCycles) {
      unsigned long cyclePos = sirenElapsed % cycleTime;
      if (cyclePos < buzzerOnTime) {
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
      }
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerSirenActive = false;
    }
  }
} 