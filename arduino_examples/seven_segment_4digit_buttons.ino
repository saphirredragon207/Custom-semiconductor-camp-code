/*
  4-Digit 7-Segment Display + Buttons Example
  Direct drive (no driver IC)
  Connect segments A-G, DP to D2-D8, D13
  Digit select pins to D9-D12 (D1=9, D2=10, D3=11, D4=12)
  Use current-limiting resistors for each segment!
*/
const int segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 13}; // A,B,C,D,E,F,G,DP
const int digitPins[4] = {12, 11, 10, 9}; // D1 (left) to D4 (right) - reversed for correct display order
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
  for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(digitPins[i], OUTPUT);

  // Diagnostic: Light up all segments on each digit one by one
  for (int i = 0; i < 4; i++) {
    // Turn off all digits
    for (int d = 0; d < 4; d++) digitalWrite(digitPins[d], HIGH);
    // Turn on all segments
    for (int s = 0; s < 8; s++) digitalWrite(segmentPins[s], HIGH);
    // Enable this digit
    digitalWrite(digitPins[i], LOW);
    delay(500);
    // Turn off this digit
    digitalWrite(digitPins[i], HIGH);
    // Turn off all segments
    for (int s = 0; s < 8; s++) digitalWrite(segmentPins[s], LOW);
  }
}
void loop() {
  int n = 9999; // Always display 9999
  for (int refresh = 0; refresh < 100; refresh++) {
    bool leading = true;
    for (int d = 0; d < 4; d++) {
      int digitValue = (n / (int)pow(10, 3 - d)) % 10;
      if (digitValue == 0 && leading && d != 3) {
        showDigit(-1, d); // Blank this digit
      } else {
        leading = false;
        showDigit(digitValue, d);
      }
      delay(1);
      clearDigits();
    }
  }
}
void showDigit(int num, int digit) {
  if (num < 0) {
    // Blank digit
    for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], LOW);
  } else {
    for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], (digitCode[num] >> i) & 1);
  }
  digitalWrite(digitPins[digit], LOW); // Enable digit (common cathode)
}
void clearDigits() {
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);
} 