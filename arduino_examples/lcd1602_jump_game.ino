/*
  LCD1602 Jumping Game with Joystick, Buzzer, and 4-Digit 7-Segment Score Display
  ------------------------------------------------------------------------------
  - Character runs on bottom row of 16x2 LCD
  - Obstacles come from the right
  - Press joystick UP to jump to top row and avoid obstacles
  - Buzzer sounds when character collides with obstacle (game over)
  - 4-digit 7-segment display shows score (increments for each obstacle avoided)

  Wiring Diagram (Arduino Uno R3):
  --------------------------------
  LCD (16x2, using LiquidCrystal):
    RS  -> D12
    E   -> D11
    D4  -> D5
    D5  -> D4
    D6  -> D3
    D7  -> D A0 (if needed, else D2)
    VSS -> GND
    VDD -> 5V
    V0  -> Potentiometer (contrast)
    RW  -> GND
    A   -> 5V (backlight)
    K   -> GND (backlight)

  4-Digit 7-Segment Display (common cathode, direct drive):
    Segments A-G, DP -> D2, D3, D4, D5, D6, D7, D8, D13
    Digit select pins -> D9, D10, D11, D12
    (Use current-limiting resistors for each segment!)
    Segments (A-G, DP):
    A: D8
    B: D9
    C: D10
    D: D13
    E: A0
    F: A2
    G: A3
    DP: A4
    Digit Select Pins:
    Digit 1: A5
    Digit 2: (If you want to use D0/D1, but best to avoid. If not enough, you may need to use a multiplexer or shift register.)
    
  Joystick:
    Y-axis  -> A1
    Button  -> D7 (active LOW)
    VCC     -> 5V
    GND     -> GND

  Buzzer:
    + -> D6
    - -> GND
*/
#include <LiquidCrystal.h>

#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define BUZZER_PIN 6
#define JOY_Y_PIN A1
#define JOY_BTN_PIN 7

// 7-segment display pins
const int segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 13}; // A,B,C,D,E,F,G,DP
const int digitPins[4] = {9, 10, 11, 12};
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

// Game symbols
byte CHAR_RUN[8] = {0b00100,0b01110,0b10101,0b00100,0b01110,0b10100,0b00100,0b01010}; // stickman
byte OBSTACLE[8] = {0b00000,0b00100,0b01110,0b11111,0b01110,0b00100,0b00000,0b00000}; // spike

// Game variables
int charRow = 1; // 1 = bottom, 0 = top
int charCol = 1; // fixed horizontal position
bool isJumping = false;
unsigned long jumpStart = 0;
const unsigned long jumpDuration = 600; // ms

#define OBSTACLE_CHAR 1
#define PLAYER_CHAR 0

struct Obstacle {
  int col;
  int row;
  bool active;
};

const int maxObstacles = 3;
Obstacle obstacles[maxObstacles];
unsigned long lastObstacleTime = 0;
const unsigned long obstacleInterval = 900; // ms
unsigned long lastMoveTime = 0;
const unsigned long moveInterval = 180; // ms

bool gameOver = false;
int score = 0;

void setup() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();
  lcd.createChar(PLAYER_CHAR, CHAR_RUN);
  lcd.createChar(OBSTACLE_CHAR, OBSTACLE);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(JOY_BTN_PIN, INPUT_PULLUP);
  // 7-segment pins
  for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(digitPins[i], OUTPUT);
  lcd.setCursor(2,0);
  lcd.print("Jump Game!");
  delay(1200);
  resetGame();
}

void resetGame() {
  for (int i=0; i<maxObstacles; i++) obstacles[i].active = false;
  charRow = 1;
  isJumping = false;
  gameOver = false;
  score = 0;
  lcd.clear();
  drawPlayer();
}

void drawPlayer() {
  lcd.setCursor(charCol, 0);
  lcd.print(" ");
  lcd.setCursor(charCol, 1);
  lcd.print(" ");
  lcd.setCursor(charCol, charRow);
  lcd.write(byte(PLAYER_CHAR));
}

void drawObstacles() {
  for (int i=0; i<maxObstacles; i++) {
    if (obstacles[i].active) {
      lcd.setCursor(obstacles[i].col, obstacles[i].row);
      lcd.write(byte(OBSTACLE_CHAR));
    }
  }
}

void clearObstacles() {
  for (int i=0; i<maxObstacles; i++) {
    if (obstacles[i].active) {
      lcd.setCursor(obstacles[i].col, obstacles[i].row);
      lcd.print(" ");
    }
  }
}

void spawnObstacle() {
  for (int i=0; i<maxObstacles; i++) {
    if (!obstacles[i].active) {
      obstacles[i].col = LCD_COLS-1;
      obstacles[i].row = 1; // always on ground
      obstacles[i].active = true;
      break;
    }
  }
}

void moveObstacles() {
  for (int i=0; i<maxObstacles; i++) {
    if (obstacles[i].active) {
      lcd.setCursor(obstacles[i].col, obstacles[i].row);
      lcd.print(" "); // clear old
      obstacles[i].col--;
      if (obstacles[i].col == charCol && obstacles[i].row == charRow) {
        // Collision!
        gameOver = true;
        digitalWrite(BUZZER_PIN, HIGH);
        lcd.setCursor(4,0);
        lcd.print("GAME OVER");
        lcd.setCursor(2,1);
        lcd.print("Press Btn");
        return;
      }
      if (obstacles[i].col < 0) {
        obstacles[i].active = false;
        score++; // Increase score for each obstacle passed
      }
    }
  }
}

void showDigit(int num, int digit) {
  for (int i = 0; i < 8; i++) digitalWrite(segmentPins[i], (digitCode[num] >> i) & 1);
  digitalWrite(digitPins[digit], LOW); // Enable digit (common cathode)
}
void clearDigits() {
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);
}
void displayScore(int num) {
  for (int d = 0; d < 4; d++) {
    int digit = (num / (int)pow(10, 3 - d)) % 10;
    showDigit(digit, d);
    delay(2);
    clearDigits();
  }
}

void loop() {
  // Always multiplex the 7-segment display for score
  displayScore(score);

  if (gameOver) {
    digitalWrite(BUZZER_PIN, HIGH);
    if (digitalRead(JOY_BTN_PIN) == LOW) {
      digitalWrite(BUZZER_PIN, LOW);
      delay(300);
      resetGame();
    }
    return;
  }

  // Handle jump
  int joyY = analogRead(JOY_Y_PIN);
  if (!isJumping && joyY < 400) { // UP
    isJumping = true;
    charRow = 0;
    jumpStart = millis();
    drawPlayer();
  }
  if (isJumping && millis() - jumpStart > jumpDuration) {
    isJumping = false;
    charRow = 1;
    drawPlayer();
  }

  // Move obstacles
  if (millis() - lastMoveTime > moveInterval) {
    clearObstacles();
    moveObstacles();
    drawObstacles();
    lastMoveTime = millis();
  }

  // Spawn new obstacles
  if (millis() - lastObstacleTime > obstacleInterval) {
    spawnObstacle();
    lastObstacleTime = millis();
  }

  drawPlayer();
} 