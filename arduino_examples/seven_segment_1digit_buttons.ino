/*
  1-Digit 7-Segment Display + Buttons Example
  - Connect segments A-G, DP to D2-D8
  - Button1: Increment (A0, active LOW)
  - Button2: Decrement (A1, active LOW)
  - Use current-limiting resistors for each segment!
*/
const int segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 13}; // A,B,C,D,E,F,G,DP
const int buttonInc = A0;
const int buttonDec = A1;
int digit = 0;
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
  pinMode(buttonInc, INPUT_PULLUP);
  pinMode(buttonDec, INPUT_PULLUP);
}
void loop() {
  showDigit(digit);
  if (digitalRead(buttonInc) == LOW) { digit++; delay(250); }
  if (digitalRead(buttonDec) == LOW) { digit--; delay(250); }
  if (digit > 9) digit = 0;
  if (digit < 0) digit = 9;
}
void showDigit(int num) {
  for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], (digitCode[num] >> i) & 1);
} 