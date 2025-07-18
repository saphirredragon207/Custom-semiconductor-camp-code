/*
  4-Digit 7-Segment Display Segment Test
  Lights up each segment (A-G, DP) one by one on the leftmost digit.
  Use this to verify segment-to-pin mapping and wiring.
*/
const int segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 13}; // A,B,C,D,E,F,G,DP
const int digitPins[4] = {12, 11, 10, 9}; // D1 (left) to D4 (right)

void setup() {
  for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(digitPins[i], OUTPUT);

  // Only enable the leftmost digit
  for (int d = 0; d < 4; d++) digitalWrite(digitPins[d], HIGH);
  digitalWrite(digitPins[0], LOW); // Enable leftmost digit

  // Light up each segment one by one
  for (int s = 0; s < 8; s++) {
    for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], LOW);
    digitalWrite(segmentPins[s], HIGH);
    delay(1000);
  }

  // After test, turn off all segments
  for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], LOW);
}

void loop() {} 