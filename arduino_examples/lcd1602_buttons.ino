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

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int buttonInc = 8;
const int buttonDec = 9;
const char* messages[] = {"Hello!", "Arduino", "LCD1602", "Edit me!", ":)", "Custom text"};
const int numMessages = sizeof(messages) / sizeof(messages[0]);
int currentIndex = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Message:");
  pinMode(buttonInc, INPUT_PULLUP);
  pinMode(buttonDec, INPUT_PULLUP);
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print("                "); // Clear line (16 spaces)
  lcd.setCursor(0, 1);
  lcd.print(messages[currentIndex]);

  if (digitalRead(buttonInc) == LOW) {
    currentIndex = (currentIndex + 1) % numMessages;
    delay(250); // Debounce
  }
  if (digitalRead(buttonDec) == LOW) {
    currentIndex = (currentIndex - 1 + numMessages) % numMessages;
    delay(250); // Debounce
  }
} 