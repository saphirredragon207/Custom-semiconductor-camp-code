/*
  Ultrasonic Rangefinder (HC-SR04) + 4-Digit 7-Segment Display Example
  - HC-SR04:
      VCC -> 5V
      GND -> GND
      TRIG -> D6
      ECHO -> D7
  - 4-digit 7-segment display:
      Segments A-G, DP -> D2-D5, D8, D9, D10, D11 (A,B,C,D,E,F,G,DP)
      Digit select pins -> D12-D13, A0-A1
      Use current-limiting resistors for each segment!
*/
const int trigPin = 6;
const int echoPin = 7;
const int segmentPins[8] = {2, 3, 4, 5, 8, 9, 10, 11}; // A,B,C,D,E,F,G,DP
const int digitPins[4] = {12, 13, A0, A1};
const byte digitCode[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(digitPins[i], OUTPUT);
}

void loop() {
  int distance = getDistanceCM();
  displayNumber(distance);
}

// Function to measure distance in cm using HC-SR04
int getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms
  int distance = duration * 0.034 / 2;
  if (distance < 0 || distance > 400) return 0; // Out of range
  return distance;
}

// Function to display a 4-digit number on the 7-segment display
void displayNumber(int num) {
  for (int d = 0; d < 4; d++) {
    int digit = (num / (int)pow(10, 3 - d)) % 10;
    showDigit(digit, d);
    delay(2);
    clearDigits();
  }
}

void showDigit(int num, int digit) {
  for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], (digitCode[num] >> i) & 1);
  digitalWrite(digitPins[digit], LOW); // Enable digit (common cathode)
}

void clearDigits() {
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);
} 