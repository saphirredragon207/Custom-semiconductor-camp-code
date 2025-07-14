/*
  MAX7219 8x8 LED Matrix + Buttons Example
  - Uses LedControl library (install from Library Manager)
  - DIN -> D12
  - CS  -> D11
  - CLK -> D10
  - Button1: Increment (A0, active LOW)
  - Button2: Decrement (A1, active LOW)
*/
#include <LedControl.h>
LedControl lc(12, 10, 11, 1); // DIN, CLK, CS, # of devices
const int buttonInc = A0;
const int buttonDec = A1;
int number = 0;
void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  pinMode(buttonInc, INPUT_PULLUP);
  pinMode(buttonDec, INPUT_PULLUP);
}
void loop() {
  lc.clearDisplay(0);
  showNumber(number);
  if (digitalRead(buttonInc) == LOW) { number++; delay(250); }
  if (digitalRead(buttonDec) == LOW) { number--; delay(250); }
  if (number > 9) number = 0;
  if (number < 0) number = 9;
}
void showNumber(int num) {
  // Simple 3x5 font for digits 0-9
  const byte font[10][5] = {
    {0x1E,0x29,0x25,0x23,0x1E}, // 0
    {0x00,0x21,0x3F,0x01,0x00}, // 1
    {0x21,0x33,0x29,0x25,0x21}, // 2
    {0x12,0x21,0x25,0x25,0x1A}, // 3
    {0x0C,0x0A,0x09,0x3F,0x08}, // 4
    {0x17,0x25,0x25,0x25,0x19}, // 5
    {0x1E,0x25,0x25,0x25,0x18}, // 6
    {0x01,0x39,0x05,0x03,0x01}, // 7
    {0x1A,0x25,0x25,0x25,0x1A}, // 8
    {0x06,0x29,0x29,0x29,0x1E}  // 9
  };
  for (int col = 0; col < 5; col++) {
    lc.setColumn(0, col+1, font[num][col]);
  }
} 