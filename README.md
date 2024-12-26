# Pong Game

## Group Members
1. Ahmed Ashraf
2. Muhammad Somaan
3. Ahmed Ali

---

## Brief Project Description
The Pong Game is a modern implementation of the classic arcade game, developed using C++ and the SFML (Simple and Fast Multimedia Library). Our project recreates the nostalgic gameplay with enhanced visuals and customizable features, while maintaining the simplicity and competitiveness of the original game. Designed for both single-player and multiplayer modes, the game offers an immersive experience for players.

---

## Features of the Game
1. **Welcome Window**:
   - Displays the title and three buttons: Play, Credits, and Exit.
   - Buttons have hover effects and functional click events.

2. **Single-player and Multiplayer Modes**:
   - Single-player mode: AI-controlled right paddle.
   - Multiplayer mode: Player-controlled right paddle with arrow keys.

3. **Credits Screen**:
   - Lists the names of the developers.
   - Includes a back button to return to the Welcome Window.

4. **Gameplay Mechanics**:
   - Smooth paddle movement controlled via keyboard.
   - Dynamic ball movement with customizable speeds.
   - Real-time score tracking for both players.

5. **Restart Functionality**:
   - Resets all game elements including paddle positions, scores, and ball states.

6. **Timer Mode** (optional):
   - Allows timed matches to add variety to gameplay.

---

## Architecture/Structure of the Game
### **Main Components**
1. **Welcome Screen**:
   - Title and buttons for navigation.
   - Managed via `show_welcome_window()` function.

2. **Credits Window**:
   - Displays developer information.
   - Managed via `show_credits_window()` function.

3. **Game Mechanics**:
   - Paddle movement handled by `handle_paddle_movement()`.
   - Ball behavior and collision detection are implemented in the game loop.

4. **Restart Functionality**:
   - Resets the game environment using `restart_game()`.

5. **Game Loop**:
   - Central logic for ball movement, scoring, and game state updates.
   - Includes rendering of game elements on the screen.

### **Key Components and Variables**
- **Window Management**: Utilizes SFML for rendering and event handling.
- **Game Objects**:
  - `left_paddle` and `right_paddle` for player paddles.
  - `balls` array for ball instances and movement logic.
- **Game States**:
  - `is_singleplayer` for mode selection.
  - `left_score` and `right_score` for score tracking.

---

## How to Run and Deploy the Project
### **Prerequisites**
1. Install a C++ compiler (e.g., GCC or MSVC).
2. Install the SFML library:
   - Follow installation instructions from the official SFML website: [https://www.sfml-dev.org](https://www.sfml-dev.org).

### **Steps to Build and Run**
1. Clone the project repository:
   ```bash
   git clone <https://github.com/ahmedashrafk13/Pong-Game>
   ```

2. Navigate to the project directory:
   ```bash
   cd Pong-Game
   ```

3. Compile the project:
   ```bash
   g++ -o Pong-Game task.cpp -lsfml-graphics -lsfml-window -lsfml-system
   ```

4. Run the executable:
   ```bash
   ./Pong-Game
   ```

### **Deployment Instructions**
1. Package the compiled executable along with necessary SFML library files.
2. Distribute the package with a README or setup instructions.
3. For Windows users, create a `.exe` file with SFML DLLs bundled.
4. For Linux users, ensure dependencies are installed or use static linking.

---

## Additional Notes
- **Customization**: Modify game parameters like ball speed or paddle size in the source code.
- **Future Enhancements**:
  - Improved AI for single-player mode.
  - Additional game modes like power-ups or obstacles.
- **Bug Reporting**: Please report issues via the project repository or contact the developers directly.

---

Enjoy the game!

