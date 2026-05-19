# Stack-Master
An Arduino-based LED stacking game using MAX7219 display and push-button controls with dynamic scoring and increasing difficulty.
# Stack Master 🎮
An Arduino-based LED stacking game built using the MAX7219 LED Matrix Display.
The game challenges players to align moving LED blocks accurately while the speed gradually increases over time.

## 📌 Project Overview

Stack Master is a reflex-based arcade game developed using Arduino and the MAX7219 LED matrix module.
The player must stop moving LED blocks at the correct position to stack them successfully.
Accurate stacking rewards the player with higher points, while incorrect alignment reduces scoring opportunities or ends the game.

The game difficulty increases dynamically by increasing block movement speed after successful moves, making the gameplay more challenging and engaging.

## ⚙️ Features

* Real-time LED gameplay
* Dynamic scoring system
* Speed increases gradually with progress
* Push-button control system
* MAX7219 LED matrix display interface
* Lightweight embedded game logic
* Serial monitor debugging support
<img width="618" height="297" alt="Screenshot 2026-05-19 at 11 13 57 AM" src="https://github.com/user-attachments/assets/a99b6461-4f47-4051-89ad-c34f33410650" />

## 🛠️ Hardware Components

| Component                  | Purpose          |
| -------------------------- | ---------------- |
| Arduino UNO                | Main controller  |
| MAX7219 LED Matrix Display | Game display     |
| Push Button                | User input       |
| Jumper Wires               | Connections      |
| Breadboard                 | Circuit assembly |

---

## 🔌 Circuit Connections

### MAX7219 Display Connections

| MAX7219 Pin | Arduino UNO Pin |
| ----------- | --------------- |
| VCC         | 5V              |
| GND         | GND             |
| DIN         | D12             |
| CS          | D10             |
| CLK         | D11             |

### Push Button Connection

| Button Pin | Arduino UNO Pin |
| ---------- | --------------- |
| One Side   | D2              |
| Other Side | GND             |

The push button uses Arduino's internal pull-up resistor configuration.

---

## 🎮 How the Game Works

1. A moving LED block continuously moves left and right across the display.
2. The player presses the button to stop and place the block.
3. Points are assigned based on how accurately the block aligns with the previous stack:

   * 4-dot alignment → 40 points
   * 3-dot alignment → 30 points
   * 2-dot alignment → 20 points
   * 1-dot alignment → 10 points
4. After every successful stack, the game speed increases gradually.
5. If the player completely misses the stack, the game ends.

---

## 🧠 Working Principle

The Arduino continuously updates the LED matrix using SPI communication.
The MAX7219 driver controls the LEDs efficiently while the Arduino processes:

* block movement,
* collision detection,
* score calculation,
* game speed adjustment,
* and player input handling.

The movement speed is controlled using a variable delay mechanism, allowing dynamic difficulty progression during gameplay.

---

## 💻 Software Used

* Arduino IDE
* MD_MAX72XX Library
* SPI Library
* Wokwi Simulator

---

## 🚀 Future Improvements

* OLED/LCD score display
* Multiplayer mode
* Sound effects using buzzer
* Joystick controls
* High-score memory storage
* Bluetooth/WiFi leaderboard support

---

## 👩‍💻 Author

Poojitha
Arduino & Embedded Systems Project
