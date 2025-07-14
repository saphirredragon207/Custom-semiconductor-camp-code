/*
  4-Digit 7-Segment Display + Buttons Example
  - Direct drive (no driver IC)
  - Connect segments A-G, DP to D2-D8
*/
  - Digit select pins to D9-D12
  - Button1: Increment (A0, active LOW)
  - Button2: Decrement (A1, active LOW)
  - Use current-limiting resistors for each segment!
*/
const int segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 13}; // A,B,C,D,E,F,G,DP
const int digitPins[4] = {9, 10, 11, 12};
const int buttonInc = A0;
const int buttonDec = A1;
int number = 0;
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
  pinMode(buttonInc, INPUT_PULLUP);
  pinMode(buttonDec, INPUT_PULLUP);
}
void loop() {
  for (int d = 0; d < 4; d++) {
    showDigit((number / (int)pow(10, 3 - d)) % 10, d);
    delay(2);
    clearDigits();
  }
  if (digitalRead(buttonInc) == LOW) { number++; delay(250); }
  if (digitalRead(buttonDec) == LOW) { number--; delay(250); }
  if (number > 9999) number = 0;
  if (number < 0) number = 9999;
}
void showDigit(int num, int digit) {
  for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], (digitCode[num] >> i) & 1);
  digitalWrite(digitPins[digit], LOW); // Enable digit (common cathode)
}
void clearDigits() {
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);
} 