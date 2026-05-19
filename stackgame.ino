#include <MD_MAX72xx.h>
#include <SPI.h>

// Define MAX7219 settings
#define MAX_DEVICES 4  // 4-in-1 Display
#define CLK_PIN 11
#define DATA_PIN 12
#define CS_PIN 10

MD_MAX72XX matrix = MD_MAX72XX(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Button Pin
#define BUTTON_PIN 2  // Push button pin (using internal pull-up)

// Grid size
#define WIDTH 8
#define HEIGHT 32  // 4 modules x 8 rows

// Game variables
int blockPos = 0;              // Current block x-position
const int BLOCK_WIDTH = 4;     // Block width in LEDs
int blockRow = 0;              // Current block row (0 = bottom)
bool blockMoving = true;       // Is block moving?
int stack[HEIGHT][WIDTH];      // Stack state (1 = LED on, 0 = off)
int stackHeight = 0;           // Highest occupied row
bool gameOver = false;         // Game over flag
unsigned long lastMove = 0;    // Last block move time
int moveDelay = 200;           // Current move delay (ms)
bool moveRight = true;         // Block movement direction
bool testMode = false;         // Test mode for display mapping
int testRow = 0;               // Current row for test mode
const bool ROTATE_DISPLAY = true; // Adjust for FC-16 rotation (rows/columns swapped)

// NEW: scoring system
int score = 0;
int comboCount = 0;
int bestCombo = 0;

const int BASE_MOVE_DELAY = 200;
const int MIN_MOVE_DELAY = 50;

void updateDisplay();
void resetGame();
void dropBlock();

void setup() {
  Serial.begin(9600);
  Serial.println("Setup started");

  matrix.begin();
  matrix.clear();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(A0));

  resetGame();
  Serial.println("Game initialized. Send 't' in Serial Monitor to enter test mode.");
}

void resetGame() {
  // Clear stack array
  for (int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      stack[row][col] = 0;
    }
  }

  blockPos = random(0, WIDTH - BLOCK_WIDTH + 1);
  blockRow = 0;
  stackHeight = 0;
  gameOver = false;
  blockMoving = true;
  moveDelay = BASE_MOVE_DELAY;
  moveRight = true;
  testMode = false;
  testRow = 0;

  // NEW: reset scoring
  score = 0;
  comboCount = 0;
  bestCombo = 0;

  updateDisplay();
  Serial.println("Game reset");
  Serial.println("Score = 0");
}

void updateDisplay() {
  matrix.clear();

  if (testMode) {
    // Test mode: Light up one row at a time
    for (int col = 0; col < WIDTH; col++) {
      if (ROTATE_DISPLAY) {
        matrix.setPoint(col, testRow, true);
      } else {
        matrix.setPoint(testRow, col, true);
      }
    }
    Serial.print("Test mode: Lighting row ");
    Serial.println(testRow);
    return;
  }

  // Draw stack
  for (int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if (stack[row][col]) {
        if (ROTATE_DISPLAY) {
          matrix.setPoint(col, row, true);
        } else {
          matrix.setPoint(row, col, true);
        }
      }
    }
  }

  // Draw moving block if game not over
  if (!gameOver && blockMoving) {
    for (int col = blockPos; col < blockPos + BLOCK_WIDTH; col++) {
      if (col >= 0 && col < WIDTH) {
        if (ROTATE_DISPLAY) {
          matrix.setPoint(col, blockRow, true);
        } else {
          matrix.setPoint(blockRow, col, true);
        }
      }
    }
  }
}

void dropBlock() {
  blockMoving = false;
  int overlapCount = 0;

  // Check how many dots of the block match the row below
  for (int col = blockPos; col < blockPos + BLOCK_WIDTH; col++) {
    if (col >= 0 && col < WIDTH) {
      if (blockRow == 0 || stack[blockRow - 1][col] == 1) {
        stack[blockRow][col] = 1;
        overlapCount++;
      }
    }
  }

  // If nothing matched, game over
  if (overlapCount == 0 && blockRow > 0) {
    gameOver = true;
    Serial.println("Game over: Missed stack");
    Serial.print("Final Score: ");
    Serial.println(score);
    return;
  }

  // Points based on exact match
  int pointsEarned = overlapCount * 10;
  score += pointsEarned;

  // Update stack height
  if (blockRow >= stackHeight) {
    stackHeight = blockRow + 1;
  }

  if (stackHeight >= HEIGHT) {
    gameOver = true;
    Serial.println("Game over: Stack reached top");
    Serial.print("Final Score: ");
    Serial.println(score);
    return;
  }

  // Next row
  blockRow++;
  blockPos = random(0, WIDTH - BLOCK_WIDTH + 1);
  blockMoving = true;

  // Slow speed increase
  moveDelay = max(130, moveDelay - 1);

  Serial.print("Matched dots: ");
  Serial.print(overlapCount);
  Serial.print(" | Points earned: ");
  Serial.print(pointsEarned);
  Serial.print(" | Total Score: ");
  Serial.println(score);
}

void loop() {
  // Check for Serial input to toggle test mode
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 't') {
      testMode = !testMode;
      if (testMode) {
        testRow = 0;
        Serial.println("Entered test mode. Send 'n' or press button to advance row, 't' to exit.");
      } else {
        Serial.println("Exited test mode");
        resetGame();
      }
    } else if (testMode && c == 'n') {
      testRow = (testRow + 1) % HEIGHT;
      Serial.print("Test mode: Advanced to row ");
      Serial.println(testRow);
    }
    updateDisplay();
  }

  if (testMode) {
    // In test mode, advance row with button press
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == LOW) {
        testRow = (testRow + 1) % HEIGHT;
        Serial.print("Test mode: Advanced to row ");
        Serial.println(testRow);
        updateDisplay();
        while (digitalRead(BUTTON_PIN) == LOW);
      }
    }
    return;
  }

  if (gameOver) {
    // Flash display and wait for button to reset
    matrix.clear();
    delay(500);
    updateDisplay();
    delay(500);

    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == LOW) {
        resetGame();
        Serial.println("Game restarted");
        while (digitalRead(BUTTON_PIN) == LOW);
      }
    }
    return;
  }

  // Move block left/right
  if (blockMoving && millis() - lastMove >= moveDelay) {
    blockPos += moveRight ? 1 : -1;

    if (blockPos <= 0) {
      blockPos = 0;
      moveRight = true;
    } else if (blockPos >= WIDTH - BLOCK_WIDTH) {
      blockPos = WIDTH - BLOCK_WIDTH;
      moveRight = false;
    }

    lastMove = millis();
    updateDisplay();
  }

  // Check button press to drop block
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Button pressed");
      dropBlock();
      updateDisplay();
      while (digitalRead(BUTTON_PIN) == LOW);
    }
  }
}
