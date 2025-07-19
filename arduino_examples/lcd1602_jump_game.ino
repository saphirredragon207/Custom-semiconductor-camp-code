/*
  LCD1602 Jumping Game with Joystick and Buzzer
  ------------------------------------------------------------------------------
  - Character runs on bottom row of 16x2 LCD
  - Obstacles come from the right
  - Press joystick UP to jump to top row and avoid obstacles
  - Buzzer sounds when character collides with obstacle (game over)

  Wiring Diagram (Arduino Uno R3):
  --------------------------------
  LCD (16x2, using LiquidCrystal):
    RS  -> D12
    E   -> D11
    D4  -> D5
    D5  -> D4
    D6  -> D3
    D7  -> D2
    VSS -> GND
    VDD -> 5V
    V0  -> Potentiometer (contrast)
    RW  -> GND
    A   -> 5V (backlight)
    K   -> GND (backlight)

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

void loop() {
  // Game over state
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