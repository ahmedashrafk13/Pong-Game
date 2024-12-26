Pong Game 
Welcome to Pong Game, a modern reimagining of the classic arcade game! Built with C++ and powered by the SFML library, this game brings smooth gameplay, interactive visuals, and exciting features that make it enjoyable for players of all skill levels.

Whether you want to challenge yourself against an AI opponent or compete with a friend in multiplayer mode, this game has something for everyone!

Features ✨
Gameplay Modes
Single-Player Mode:

Face off against a computer-controlled paddle with adjustable difficulty levels.
Hone your skills and test your reflexes!
Multiplayer Mode:

Play with a friend on the same device, with split-screen controls.
Perfect for competitive matches and friendly rivalries.
Customization
Score Limit Options: Choose the winning score (15, 25, or 40 points).
Timer Mode: Play for a fixed duration and compete to achieve the highest score.
User Interface
Welcome Screen: Navigate through intuitive options for playing, viewing credits, or exiting the game.
Mode Selection Screen: Choose your desired gameplay mode and set the score limit or timer mode.
Credits Screen: View acknowledgments for the game development and contributors.
Additional Features
Restart Functionality: Easily reset the game and return to the welcome screen.
Dynamic Button Effects: Buttons change color when hovered over or clicked, providing a polished and interactive experience.
Smooth Ball Physics: The ball bounces realistically off paddles and walls, creating an engaging challenge.
Installation and Setup 🛠️
Follow these steps to install and run the Pong Game on your system:

1. Prerequisites
C++ Compiler: Ensure you have a compiler like GCC or MSVC installed.
SFML Library: Download and install SFML (Simple and Fast Multimedia Library).
2. Clone the Repository
Clone the repository from GitHub:

bash
Copy code
git clone https://github.com/yourusername/pong-game.git  
cd pong-game  
3. Build the Game
Using Makefile:
bash
Copy code
make  
Manual Compilation:
bash
Copy code
g++ -o pong main.cpp -lsfml-graphics -lsfml-window -lsfml-system  
4. Run the Game
Execute the game binary:

bash
Copy code
./pong  
How to Play 🎮
Controls

Player 1:
Move Up: W
Move Down: S

Player 2:
Move Up: Up Arrow
Move Down: Down Arrow

Exit: Press Escape to exit to the main menu.

Objective

Single-Player Mode: Defeat the AI paddle by scoring the target number of points before it does.

Multiplayer Mode: Outplay your opponent by reaching the score limit or outscoring them in timer mode.


Development 🛠️
Code Structure
task.cpp: The entry point of the game, managing game logic and UI rendering.

Game Logic: Handles paddle movement, ball physics, scoring, and resetting.

UI Components: Implements the welcome screen, mode selection window, and credits screen.

Key Features Implemented

Paddle Movement: Responsive controls for both single-player and multiplayer modes.

Ball Physics: Realistic collision detection and response for engaging gameplay.

Interactive UI: Hover and click effects for buttons, enhancing user experience.

Fork this repository.
Create a new branch for your feature:
bash
Copy code
git checkout -b feature-name  
Commit your changes and push to your branch.
Open a Pull Request with a description of your changes.
License 📜
This project is licensed under the MIT License. Feel free to use, modify, and distribute it as long as proper credit is given.

