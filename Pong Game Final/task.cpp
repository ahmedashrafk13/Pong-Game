#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib> // For rand()
#include <vector>

using namespace std;
using namespace sf;

// Constants
const int screenwidth = 800;             // Screen width
const int screenheight = 600;            // Screen height
const float paddlewidth = 20.0f;         // Paddle width
const float paddleheight = 100.0f;       // Paddle height
const float ballradius = 10.0f;          // Radius of the ball                  
const float initial_ballspeed = 0.5f;     // Initial ball speed
                    

// Global variables
float ball_speed = 0;                   // Ball speed (initially set to 0)
int targetscore = 15;                   // Target score for winning
RectangleShape left_paddle, right_paddle;  // Paddle objects for left and right players
vector<CircleShape> balls;               // Vector of balls for multiplayer/multi-ball mode
vector<float> ballSpeedX, ballSpeedY;    // X and Y speeds for each ball
int left_score = 0, right_score = 0;       // Scores for left and right players
Font game_font;                           // Font for displaying text
int missed_balls = 0;                     // Count of missed balls
bool is_singleplayer = false;             // Flag for single-player mode
int selected_mode = 0;                   // Selected mode (single/multiplayer)
int difficulty = 0;                      // Difficulty level (0 = Easy, 1 = Medium, 2 = Hard)
Clock game_clock;                         // Timer clock for timing
const int time_limit = 60;                // Time limit in seconds for Timer Mode
bool is_timermode = false;                // Indicates if Timer Mode is active
float paddle_speed = 1.0f;                // Speed of the paddles

// Function prototypes
void initialize_game();                   // Initializes game settings
void reset_ball(int index);               // Resets a specific ball to the center
void handle_paddle_movement(float paddleSpeed); // Handles paddle movement
float adjust_paddle_speed();               // Adjusts paddle speed dynamically
void draw_score(RenderWindow& window, int score, bool isLeft); // Draws score on the screen
void handleinput(RenderWindow& window); // Handles player input
void check_collisions(RenderWindow& window); // Checks and handles collisions
void show_welcome_window(RenderWindow& window, Font& gameFont); // Displays the welcome window
void show_mode_selectionwindow(RenderWindow& window, Font& gameFont); // Displays mode selection window
void ai_movement();                       // Handles AI paddle movement in single-player mode
void adjust_ballspeed(size_t ballIndex);  // Dynamically adjusts ball speed
void show_difficulty_window(RenderWindow& window, Font& gameFont); // Displays difficulty selection window
void check_game_end(RenderWindow& window); // Checks if the game has ended
void display_winner(RenderWindow& window, const string& winner, int leftScore, int rightScore); // Displays winner
void restart_game(RenderWindow& window, Font& gameFont); // Restarts the game

SoundBuffer hitBuffer;               // Sound buffer for paddle hit sound
Sound hitSound;                      // Sound effect for paddle hits

int main() {
    // Load font for displaying text
    if (!game_font.loadFromFile("Arial.ttf")) {
        cout << "Error: Could not load font!" << endl;
        return -1;
    }

    // Load background music
    Music backgroundMusic;
    if (!backgroundMusic.openFromFile("pong_music.ogg.opus")) {
        std::cerr << "Error loading background music!" << std::endl;
    }

    // Load paddle hit sound effect
    if (!hitBuffer.loadFromFile("ballsound.ogg")) {
        std::cerr << "Error: Could not load paddle hit sound!" << std::endl;
    }
    hitSound.setVolume(100);
    hitSound.setBuffer(hitBuffer);


    // Configure background music
    backgroundMusic.setLoop(true);       // Loop the music
    backgroundMusic.setVolume(50);       // Set volume
    backgroundMusic.play();              // Start playing music

    RenderWindow window(VideoMode(screenwidth, screenheight), "Pong Game");

    // Show welcome screen and difficulty selection before starting the game
    show_welcome_window(window, game_font);
    show_difficulty_window(window, game_font);

    // Timer text configuration
    Text timerText;
    timerText.setFont(game_font);
    timerText.setCharacterSize(30);
    timerText.setFillColor(Color::White);
    timerText.setPosition(380, 10);      // Position of timer text

    // Main game loop
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close(); // Close window if the user exits
        }

        // Timer Mode logic
        if (is_timermode) {
            Time elapsedTime = game_clock.getElapsedTime();
            int remainingTime = time_limit - static_cast<int>(elapsedTime.asSeconds());
            if (remainingTime <= 0) { // If time is up
                string winner;
                if (left_score > right_score) {
                    winner = "Left Player Wins!";
                }
                else if (right_score > left_score) {
                    winner = is_singleplayer ? "Left Player Wins" : "AI Wins!";
                }
                else {
                    winner = "It's a Tie!";
                }

                display_winner(window, winner, left_score, right_score);
                break; // End the game
            }

            // Update timer display
            int minutes = remainingTime / 60;
            int seconds = remainingTime % 60;
            timerText.setString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds));
        }

        // Check if any player has reached the target score
        if (left_score >= targetscore || right_score >= targetscore) {
            cout << "Game Over! Final Score: Left " << left_score << " - Right " << right_score << endl;
            window.close();
            break;
        }

        // Handle input and game logic
        handleinput(window);

        // Clear screen and redraw game objects
        window.clear(Color::Black);
        window.draw(left_paddle);
        window.draw(right_paddle);

        // Draw balls
        for (const auto& ball : balls) {
            window.draw(ball);
        }

        // Draw scores
        draw_score(window, left_score, true);
        draw_score(window, right_score, false);

        // Draw timer if active
        if (is_timermode) {
            window.draw(timerText);
        }

        // Update ball movement and speed
        for (size_t i = 0; i < balls.size(); ++i) {
            adjust_ballspeed(i); // Adjust speed dynamically
            balls[i].move(ballSpeedX[i], ballSpeedY[i]); // Move ball
        }

        // Check collisions and game end conditions
        check_collisions(window);
        check_game_end(window);

        // Display everything
        window.display();
    }

    return 0;
}



void initialize_game() {
    // Set up the left paddle
    left_paddle.setSize(Vector2f(paddlewidth, paddleheight)); // Set the size of the paddle
    left_paddle.setFillColor(Color::White); // Set the paddle color to white
    left_paddle.setPosition(50.0f, screenheight / 2 - paddleheight / 2); // Position it near the left edge, vertically centered

    // Set up the right paddle
    right_paddle.setSize(Vector2f(paddlewidth, paddleheight)); // Set the size of the paddle
    right_paddle.setFillColor(Color::White); // Set the paddle color to white
    right_paddle.setPosition(screenwidth - 50.0f - paddlewidth, screenheight / 2 - paddleheight / 2); // Position it near the right edge, vertically centered

    // Timer Mode-specific setup
    if (is_timermode) {
        targetscore = 1000; // Set the target score for Timer Mode
        game_clock.restart(); // Restart the game timer
    }

    // Set the initial ball speed based on the selected difficulty level
    float initialspeed;
    switch (difficulty) {
    case 0: 
        initialspeed = 0.8f; // Easy difficulty
        break;
    case 1: 
        initialspeed = 1.2f; // Medium difficulty
        break;
    case 2: 
        initialspeed = 1.6f; // Hard difficulty
        break;
    default: 
        initialspeed = 1.3f; // Default to medium difficulty
        break;
    }

    // Prepare the game with one initial ball
    balls.clear(); // Clear any existing balls
    ballSpeedX.clear(); // Clear the X-axis speed of the balls
    ballSpeedY.clear(); // Clear the Y-axis speed of the balls

    // Create a new ball and set its initial properties
    CircleShape newBall(ballradius);
    newBall.setFillColor(Color::White); // Set the ball color to white
    newBall.setPosition(screenwidth / 2 - ballradius, screenheight / 2 - ballradius); // Position it at the center of the screen
    balls.push_back(newBall); // Add the ball to the list of balls

    // Set initial ball speed in both X and Y directions
    ballSpeedX.push_back((rand() % 2 == 0 ? initialspeed : -initialspeed)); // Randomly choose left or right direction
    ballSpeedY.push_back((rand() % 2 == 0 ? initialspeed : -initialspeed)); // Randomly choose up or down direction

    // Reset the game scores and counters
    left_score = 0; // Reset left player score
    right_score = 0; // Reset right player score
    missed_balls = 0; // Reset the number of missed balls
}

void reset_ball(int index) {
    // If the specified ball index is out of range, create a new ball
    if (index >= balls.size()) {
        balls.emplace_back(CircleShape(ballradius)); // Add a new ball with the specified radius
        ballSpeedX.emplace_back(0.0f); // Initialize X-axis speed to 0
        ballSpeedY.emplace_back(0.0f); // Initialize Y-axis speed to 0
    }

    // Reset the ball's position and appearance
    balls[index].setFillColor(Color::White); // Set the ball color to white
    balls[index].setPosition(screenwidth / 2.0f - ballradius, screenheight / 2.0f - ballradius); // Place the ball at the center of the screen
}

void show_difficulty_window(RenderWindow& window, Font& gameFont) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Title Text
    Text titleText;
    titleText.setFont(gameFont);
    titleText.setString("Select Difficulty"); // Set the title text
    titleText.setCharacterSize(50); // Set the font size
    titleText.setFillColor(Color::White); // Set the text color to white
    titleText.setPosition((screenWidth - titleText.getLocalBounds().width) / 2, 100); // Center the title at the top

    // Easy Button Setup
    RectangleShape easyButton(Vector2f(200, 50)); // Create a rectangular button
    easyButton.setFillColor(Color::Green); // Set button color to green
    easyButton.setPosition(screenWidth / 2 - 100, screenHeight / 2 - 70); // Center the button

    Text easyText;
    easyText.setFont(gameFont); // Set the font
    easyText.setString("Easy"); // Set button text
    easyText.setCharacterSize(30); // Set font size
    easyText.setFillColor(Color::White); // Set text color
    easyText.setPosition(easyButton.getPosition().x + 60, easyButton.getPosition().y + 5); // Position text on the button

    // Medium Button Setup
    RectangleShape mediumButton(Vector2f(200, 50)); // Create a rectangular button
    mediumButton.setFillColor(Color::Yellow); // Set button color to yellow
    mediumButton.setPosition(screenWidth / 2 - 100, screenHeight / 2 + 20); // Center the button

    Text mediumText;
    mediumText.setFont(gameFont); // Set the font
    mediumText.setString("Medium"); // Set button text
    mediumText.setCharacterSize(30); // Set font size
    mediumText.setFillColor(Color::Black); // Set text color
    mediumText.setPosition(mediumButton.getPosition().x + 45, mediumButton.getPosition().y + 5); // Position text on the button

    // Hard Button Setup
    RectangleShape hardButton(Vector2f(200, 50)); // Create a rectangular button
    hardButton.setFillColor(Color::Red); // Set button color to red
    hardButton.setPosition(screenWidth / 2 - 100, screenHeight / 2 + 110); // Center the button

    Text hardText;
    hardText.setFont(gameFont); // Set the font
    hardText.setString("Hard"); // Set button text
    hardText.setCharacterSize(30); // Set font size
    hardText.setFillColor(Color::White); // Set text color
    hardText.setPosition(hardButton.getPosition().x + 55, hardButton.getPosition().y + 5); // Position text on the button

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close(); // Close the window if the close button is pressed
                return;
            }
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);

                // Check if the Easy button is clicked
                if (easyButton.getGlobalBounds().contains(mousePosition)) {
                    difficulty = 0; // Set difficulty to Easy
                    initialize_game(); // Initialize the game
                    return;
                }
                // Check if the Medium button is clicked
                else if (mediumButton.getGlobalBounds().contains(mousePosition)) {
                    difficulty = 1; // Set difficulty to Medium
                    initialize_game(); // Initialize the game
                    return;
                }
                // Check if the Hard button is clicked
                else if (hardButton.getGlobalBounds().contains(mousePosition)) {
                    difficulty = 2; // Set difficulty to Hard
                    initialize_game(); // Initialize the game
                    return;
                }
            }
        }

        // Render the difficulty selection window
        window.clear();
        window.draw(titleText); // Draw the title text
        window.draw(easyButton); // Draw the Easy button
        window.draw(easyText); // Draw the Easy button text
        window.draw(mediumButton); // Draw the Medium button
        window.draw(mediumText); // Draw the Medium button text
        window.draw(hardButton); // Draw the Hard button
        window.draw(hardText); // Draw the Hard button text
        window.display();
    }
}

void adjust_ballspeed(size_t ballIndex) {
    // Set the ball speed based on the difficulty level
    switch (difficulty) {
    case 0: ball_speed = 0.8f; break; // Easy difficulty
    case 2: ball_speed = 1.6f; break; // Hard difficulty
    default: ball_speed = 1.2f; break; // Medium or default difficulty
    }

    // Apply the speed adjustment to all balls
    for (size_t i = 0; i < balls.size(); ++i) {
        // Normalize the initial direction vector
        float directionX = ballSpeedX[i] / std::sqrt(ballSpeedX[i] * ballSpeedX[i] + ballSpeedY[i] * ballSpeedY[i]);
        float directionY = ballSpeedY[i] / std::sqrt(ballSpeedX[i] * ballSpeedX[i] + ballSpeedY[i] * ballSpeedY[i]);

        // Scale the direction vector to the target speed
        ballSpeedX[i] = directionX * ball_speed;
        ballSpeedY[i] = directionY * ball_speed;
    }
}

float adjust_paddle_speed() {
    // Adjust paddle speed based on the current difficulty level
    switch (difficulty) {
    case 0: // Easy difficulty
        return paddle_speed; // Slower paddle for easier gameplay
    case 2: // Hard difficulty
        return paddle_speed + 1.5f; // Faster paddle for harder gameplay
    default: // Medium or default difficulty
        return paddle_speed + 0.8f; // Default speed for medium difficulty
    }
}


void display_winner(RenderWindow& window, const string& winner, int leftScore, int rightScore) {
    // Prepare the winner text
    Text winnerText;
    winnerText.setFont(game_font); // Set the font for the winner text
    winnerText.setCharacterSize(60); // Set the font size for visibility
    winnerText.setFillColor(Color::Yellow); // Set the text color to yellow
    winnerText.setString(winner); // Set the winner string

    // Center the winner text
    FloatRect winnerBounds = winnerText.getLocalBounds();
    winnerText.setPosition((screenwidth - winnerBounds.width) / 2.0f, screenheight / 3.0f - 50.0f);

    // Prepare the score text
    Text scoreText;
    scoreText.setFont(game_font); // Set the font for the score text
    scoreText.setCharacterSize(40); // Set font size for emphasis
    scoreText.setFillColor(Color::White); // Set text color to white
    scoreText.setString("Final Score:\nLeft Player: " + std::to_string(leftScore) + "\nRight Player: " + std::to_string(rightScore));

    // Center the score text
    FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setPosition((screenwidth - scoreBounds.width) / 2.0f, screenheight / 2.0f);

    // Display the winner screen
    window.clear(Color::Black); // Set a black background
    window.draw(winnerText);    // Draw the winner text
    window.draw(scoreText);     // Draw the score text
    window.display();

    // Pause to allow players to view the winner screen
    sleep(seconds(5));
    restart_game(window, game_font); // Restart the game
    window.close(); // Close the window after displaying the winner
}

void handleinput(RenderWindow& window) {
    // Adjust paddle speed based on difficulty
    float currentPaddleSpeed = adjust_paddle_speed();

    // Handle player paddle movements
    handle_paddle_movement(currentPaddleSpeed);

    // Handle AI paddle movement in single-player mode
    if (is_singleplayer) {
        ai_movement();
    }

    // Additional inputs (e.g., pause, quit) can be handled here
}

void ai_movement() {
    if (!is_singleplayer || balls.empty()) return; // Return if not single-player or no balls present

    // Get the center positions of the ball and the AI paddle
    float ballCenter = balls[0].getPosition().y + ballradius;
    float paddleCenter = right_paddle.getPosition().y + paddleheight / 2.0f;

    // Introduce reaction delay for Easy mode
    static Clock reactionClock; // Reaction timer
    float reactionTime = (difficulty == 0) ? 0.01f : 0.03f; // Easy: slower, Medium/Hard: faster

    if (reactionClock.getElapsedTime().asSeconds() < reactionTime && difficulty == 0) {
        return; // Skip movement until reaction time has passed
    }
    if (difficulty == 0) reactionClock.restart();

    // Set AI behavior parameters based on difficulty
    float speed;
    float reactionMargin;
    float predictionError = 0.0f; // Introduce intentional inaccuracy

    switch (difficulty) {
    case 0: // Easy
        speed = 6.0f;
        reactionMargin = 50.0f;
        predictionError = 50.0f;
        break;
    case 1: // Medium
        speed = 6.0f;
        reactionMargin = 20.0f;
        predictionError = 15.0f;
        break;
    case 2: // Hard
        speed = 8.0f;
        reactionMargin = 5.0f;
        predictionError = 5.0f;
        break;
    default: // Default to Medium
        speed = 6.0f;
        reactionMargin = 20.0f;
        predictionError = 15.0f;
        break;
    }

    // Add randomness to simulate AI misjudgment
    ballCenter += (rand() % 2 == 0 ? -predictionError : predictionError);

    // Move the AI paddle within screen boundaries
    if (paddleCenter < ballCenter - reactionMargin && right_paddle.getPosition().y + paddleheight < screenheight) {
        right_paddle.move(0, speed); // Move paddle down
    }
    else if (paddleCenter > ballCenter + reactionMargin && right_paddle.getPosition().y > 0) {
        right_paddle.move(0, -speed); // Move paddle up
    }
}

void check_collisions(RenderWindow& window) {
    for (size_t i = 0; i < balls.size(); ++i) {
        // Ball collision with top and bottom walls
        if (balls[i].getPosition().y <= 0 || balls[i].getPosition().y + 2 * ballradius >= screenheight) {
            ballSpeedY[i] = -ballSpeedY[i]; // Reverse Y direction
        }

        // Ball collision with paddles
        if (balls[i].getGlobalBounds().intersects(left_paddle.getGlobalBounds())) {
            ballSpeedX[i] = abs(ballSpeedX[i]); // Bounce right
            hitSound.play(); // Play hit sound
        }
        else if (balls[i].getGlobalBounds().intersects(right_paddle.getGlobalBounds())) {
            ballSpeedX[i] = -abs(ballSpeedX[i]); // Bounce left
            hitSound.play(); // Play hit sound
        }

        // Check if ball passes a paddle (score points)
        if (balls[i].getPosition().x <= 0) { // Left wall
            right_score++;
            reset_ball(i);
        }
        else if (balls[i].getPosition().x + 2 * ballradius >= screenwidth) { // Right wall
            left_score++;
            reset_ball(i);
        }

        // End the game if the target score is reached
        if (left_score >= targetscore || right_score >= targetscore) {
            display_winner(window, left_score >= targetscore ? "Left Player Wins!" : (is_singleplayer ? "AI Wins!" : "Right Player Wins!"), left_score, right_score);
            return;
        }
    }
}

void draw_score(RenderWindow& window, int score, bool isLeft) {
    Text scoreText;
    scoreText.setFont(game_font); // Set the font
    scoreText.setCharacterSize(24); // Set font size
    scoreText.setFillColor(Color::White); // Set text color
    scoreText.setString(std::to_string(score)); // Display the score as text

    // Position the score text for left or right player
    if (isLeft) {
        scoreText.setPosition(50, 20);
    }
    else {
        scoreText.setPosition(screenwidth - 100, 20);
    }

    window.draw(scoreText); // Draw the score on the screen
}

void check_game_end(RenderWindow& window) {
    if (!is_timermode) {
        // End the game based on score
        if (left_score >= targetscore || right_score >= targetscore) {
            string winner = left_score >= targetscore ? "Player 1 Wins!" : (is_singleplayer ? "AI Wins!" : "Player 2 Wins!");
            display_winner(window, winner, left_score, right_score);
        }
    }
}


void show_mode_selectionwindow(RenderWindow& window, Font& gameFont) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    bool isModeSelected = false;

    // Title Text
    Text titleText;
    titleText.setFont(gameFont);
    titleText.setString("Select Mode");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color::White);
    titleText.setStyle(Text::Bold);
    titleText.setOutlineColor(Color::Black);
    titleText.setOutlineThickness(2);
    titleText.setPosition((screenWidth - titleText.getLocalBounds().width) / 2, 30);

    // Buttons with normal, hover, and click colors
    struct Button {
        RectangleShape shape;
        Text text;
        Color normalColor;
        Color hoverColor;
        Color clickColor;
        bool isClicked = false;
    };

    // Single Player Button
    Button singlePlayerButton;
    singlePlayerButton.shape = RectangleShape(Vector2f(250, 60));
    singlePlayerButton.normalColor = Color(50, 70, 120);
    singlePlayerButton.hoverColor = Color(30, 50, 100);
    singlePlayerButton.clickColor = Color(20, 40, 80);
    singlePlayerButton.shape.setFillColor(singlePlayerButton.normalColor);
    singlePlayerButton.shape.setOutlineColor(Color::White);
    singlePlayerButton.shape.setOutlineThickness(2);
    singlePlayerButton.shape.setPosition(screenWidth / 2 - 125, 100);
    singlePlayerButton.text.setFont(gameFont);
    singlePlayerButton.text.setString("Single Player");
    singlePlayerButton.text.setCharacterSize(30);
    singlePlayerButton.text.setFillColor(Color::White);
    singlePlayerButton.text.setPosition(singlePlayerButton.shape.getPosition().x + 30, singlePlayerButton.shape.getPosition().y + 10);

    // Multiplayer Button
    Button multiplayerButton;
    multiplayerButton.shape = RectangleShape(Vector2f(250, 60));
    multiplayerButton.normalColor = Color(150, 30, 80);
    multiplayerButton.hoverColor = Color(120, 20, 60);
    multiplayerButton.clickColor = Color(100, 10, 40);
    multiplayerButton.shape.setFillColor(multiplayerButton.normalColor);
    multiplayerButton.shape.setOutlineColor(Color::White);
    multiplayerButton.shape.setOutlineThickness(2);
    multiplayerButton.shape.setPosition(screenWidth / 2 - 125, 180);
    multiplayerButton.text.setFont(gameFont);
    multiplayerButton.text.setString("Multiplayer");
    multiplayerButton.text.setCharacterSize(30);
    multiplayerButton.text.setFillColor(Color::White);
    multiplayerButton.text.setPosition(multiplayerButton.shape.getPosition().x + 40, multiplayerButton.shape.getPosition().y + 10);

    // Score Limit Options Title
    Text scoreLimitTitle;
    scoreLimitTitle.setFont(gameFont);
    scoreLimitTitle.setString("Set Score Limit or Timer Mode:");
    scoreLimitTitle.setCharacterSize(30);
    scoreLimitTitle.setFillColor(Color::White);
    scoreLimitTitle.setOutlineColor(Color::Black);
    scoreLimitTitle.setOutlineThickness(1);
    scoreLimitTitle.setPosition((screenWidth - scoreLimitTitle.getLocalBounds().width) / 2, 280);

    // Score Limit Buttons
    Button score15Button;
    score15Button.shape = RectangleShape(Vector2f(80, 50));
    score15Button.normalColor = Color(30, 70, 30);
    score15Button.hoverColor = Color(20, 50, 20);
    score15Button.clickColor = Color(10, 40, 10);
    score15Button.shape.setFillColor(score15Button.normalColor);
    score15Button.shape.setOutlineColor(Color::White);
    score15Button.shape.setOutlineThickness(2);
    score15Button.shape.setPosition(screenWidth / 2 - 130, 330);
    score15Button.text.setFont(gameFont);
    score15Button.text.setString("15");
    score15Button.text.setCharacterSize(20);
    score15Button.text.setFillColor(Color::White);
    score15Button.text.setPosition(score15Button.shape.getPosition().x + 25, score15Button.shape.getPosition().y + 10);

    Button score25Button;
    score25Button.shape = RectangleShape(Vector2f(80, 50));
    score25Button.normalColor = Color(120, 120, 0);
    score25Button.hoverColor = Color(100, 100, 0);
    score25Button.clickColor = Color(80, 80, 0);
    score25Button.shape.setFillColor(score25Button.normalColor);
    score25Button.shape.setOutlineColor(Color::White);
    score25Button.shape.setOutlineThickness(2);
    score25Button.shape.setPosition(screenWidth / 2 - 40, 330);
    score25Button.text.setFont(gameFont);
    score25Button.text.setString("25");
    score25Button.text.setCharacterSize(20);
    score25Button.text.setFillColor(Color::White);
    score25Button.text.setPosition(score25Button.shape.getPosition().x + 25, score25Button.shape.getPosition().y + 10);

    Button score40Button;
    score40Button.shape = RectangleShape(Vector2f(80, 50));
    score40Button.normalColor = Color(120, 20, 0);
    score40Button.hoverColor = Color(100, 10, 0);
    score40Button.clickColor = Color(80, 0, 0);
    score40Button.shape.setFillColor(score40Button.normalColor);
    score40Button.shape.setOutlineColor(Color::White);
    score40Button.shape.setOutlineThickness(2);
    score40Button.shape.setPosition(screenWidth / 2 + 50, 330);
    score40Button.text.setFont(gameFont);
    score40Button.text.setString("40");
    score40Button.text.setCharacterSize(20);
    score40Button.text.setFillColor(Color::White);
    score40Button.text.setPosition(score40Button.shape.getPosition().x + 25, score40Button.shape.getPosition().y + 10);

    Button timerModeButton;
    timerModeButton.shape = RectangleShape(Vector2f(250, 60));
    timerModeButton.normalColor = Color(0, 100, 100);
    timerModeButton.hoverColor = Color(0, 80, 80);
    timerModeButton.clickColor = Color(0, 60, 60);
    timerModeButton.shape.setFillColor(timerModeButton.normalColor);
    timerModeButton.shape.setOutlineColor(Color::White);
    timerModeButton.shape.setOutlineThickness(2);
    timerModeButton.shape.setPosition(screenWidth / 2 - 125, 410);
    timerModeButton.text.setFont(gameFont);
    timerModeButton.text.setString("Timer Mode");
    timerModeButton.text.setCharacterSize(30);
    timerModeButton.text.setFillColor(Color::White);
    timerModeButton.text.setPosition(timerModeButton.shape.getPosition().x + 40, timerModeButton.shape.getPosition().y + 10);

    auto updateButtonState = [&](Button& button, const Vector2f& mousePos, bool isPressed) {
        if (button.shape.getGlobalBounds().contains(mousePos)) {
            button.shape.setFillColor(isPressed ? button.clickColor : button.hoverColor);
            button.isClicked = isPressed;
        }
        else {
            button.shape.setFillColor(button.normalColor);
            button.isClicked = false;
        }
        };

    while (window.isOpen()) {
        Event event;
        Vector2f mousePosition = Vector2f(Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return;
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                updateButtonState(singlePlayerButton, mousePosition, true);
                updateButtonState(multiplayerButton, mousePosition, true);
                updateButtonState(score15Button, mousePosition, true);
                updateButtonState(score25Button, mousePosition, true);
                updateButtonState(score40Button, mousePosition, true);
                updateButtonState(timerModeButton, mousePosition, true);

                if (singlePlayerButton.isClicked) {
                    is_singleplayer = true;
                    show_difficulty_window(window, gameFont);
                    return;
                }
                if (multiplayerButton.isClicked) {
                    is_singleplayer = false;
                    show_difficulty_window(window, gameFont);
                    return;
                }
                if (score15Button.isClicked) {
                    is_timermode = false;
                    targetscore = 15;
                }
                if (score25Button.isClicked) {
                    is_timermode = false;
                    targetscore = 25;
                }
                if (score40Button.isClicked) {
                    is_timermode = false;
                    targetscore = 40;
                }
                if (timerModeButton.isClicked) {
                    is_timermode = true;
                    targetscore = 1000;
                }
            }

            if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
                updateButtonState(singlePlayerButton, mousePosition, false);
                updateButtonState(multiplayerButton, mousePosition, false);
                updateButtonState(score15Button, mousePosition, false);
                updateButtonState(score25Button, mousePosition, false);
                updateButtonState(score40Button, mousePosition, false);
                updateButtonState(timerModeButton, mousePosition, false);
            }
        }

        updateButtonState(singlePlayerButton, mousePosition, Mouse::isButtonPressed(Mouse::Left));
        updateButtonState(multiplayerButton, mousePosition, Mouse::isButtonPressed(Mouse::Left));
        updateButtonState(score15Button, mousePosition, Mouse::isButtonPressed(Mouse::Left));
        updateButtonState(score25Button, mousePosition, Mouse::isButtonPressed(Mouse::Left));
        updateButtonState(score40Button, mousePosition, Mouse::isButtonPressed(Mouse::Left));
        updateButtonState(timerModeButton, mousePosition, Mouse::isButtonPressed(Mouse::Left));

        // Render UI
        window.clear(Color::Black);
        window.draw(titleText);

        auto drawButton = [&](Button& button) {
            window.draw(button.shape);
            window.draw(button.text);
            };

        drawButton(singlePlayerButton);
        drawButton(multiplayerButton);
        window.draw(scoreLimitTitle);
        drawButton(score15Button);
        drawButton(score25Button);
        drawButton(score40Button);
        drawButton(timerModeButton);

        window.display();
    }
}

void handle_paddle_movement(float paddleSpeed) {
    // Player 1 paddle controls
    // Move paddle up if 'W' is pressed and it is not at the top of the screen
    if (Keyboard::isKeyPressed(Keyboard::W) && left_paddle.getPosition().y > 0) {
        left_paddle.move(0, -paddleSpeed);
    }
    // Move paddle down if 'S' is pressed and it is not at the bottom of the screen
    if (Keyboard::isKeyPressed(Keyboard::S) && left_paddle.getPosition().y < screenheight - paddleheight) {
        left_paddle.move(0, paddleSpeed);
    }

    // Player 2 paddle controls (only in multiplayer mode)
    if (!is_singleplayer) {
        // Move paddle up if 'Up Arrow' is pressed and it is not at the top of the screen
        if (Keyboard::isKeyPressed(Keyboard::Up) && right_paddle.getPosition().y > 0) {
            right_paddle.move(0, -paddleSpeed);
        }
        // Move paddle down if 'Down Arrow' is pressed and it is not at the bottom of the screen
        if (Keyboard::isKeyPressed(Keyboard::Down) && right_paddle.getPosition().y < screenheight - paddleheight) {
            right_paddle.move(0, paddleSpeed);
        }
    }
}

void show_credits_window(RenderWindow& window, Font& gameFont) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Setup for the "Back" button
    RectangleShape backButton(Vector2f(200, 50));
    backButton.setFillColor(Color(34, 139, 34)); // Dark Green color
    backButton.setPosition(screenWidth / 2 - 100, screenHeight - 100);

    Text backText;
    backText.setFont(gameFont);
    backText.setString("Back");
    backText.setCharacterSize(30);
    backText.setFillColor(Color::White);
    // Center the text within the button
    backText.setPosition(
        backButton.getPosition().x + (backButton.getSize().x - backText.getLocalBounds().width) / 2,
        backButton.getPosition().y + (backButton.getSize().y - backText.getLocalBounds().height) / 4
    );

    // Title text for the Credits screen
    Text creditsTitle;
    creditsTitle.setFont(gameFont);
    creditsTitle.setString("Credits");
    creditsTitle.setCharacterSize(50);
    creditsTitle.setFillColor(Color(135, 206, 250)); // Sky Blue color
    creditsTitle.setPosition(
        (screenWidth - creditsTitle.getLocalBounds().width) / 2,
        50
    );

    // List of credits
    Text creditsInfo;
    creditsInfo.setFont(gameFont);
    creditsInfo.setString(
        "Developed by:\n\n"
        "1. Ahmed Ashraf\n"
        "2. Muhammad Somaan\n"
        "3. Ahmed Ali"
    );
    creditsInfo.setCharacterSize(35);
    creditsInfo.setFillColor(Color::White);
    creditsInfo.setPosition(
        (screenWidth - creditsInfo.getLocalBounds().width) / 2,
        150
    );

    // Decorative horizontal lines
    RectangleShape topLine(Vector2f(screenWidth - 100, 5));
    topLine.setFillColor(Color::White);
    topLine.setPosition(50, 120);

    RectangleShape bottomLine(Vector2f(screenWidth - 100, 5));
    bottomLine.setFillColor(Color::White);
    bottomLine.setPosition(50, screenHeight - 150);

    // Event loop to handle interactions on the Credits screen
    while (window.isOpen()) {
        Event creditsEvent;
        while (window.pollEvent(creditsEvent)) {
            if (creditsEvent.type == Event::Closed) {
                // Close the window
                window.close();
                exit(0);
            }

            if (creditsEvent.type == Event::MouseButtonPressed && creditsEvent.mouseButton.button == Mouse::Left) {
                // Check if the "Back" button was clicked
                Vector2f mousePosition(creditsEvent.mouseButton.x, creditsEvent.mouseButton.y);
                if (backButton.getGlobalBounds().contains(mousePosition)) {
                    // Navigate back to the welcome screen
                    show_welcome_window(window, gameFont);
                    return;
                }
            }
        }

        // Render the Credits screen
        window.clear(Color::Black);
        window.draw(creditsTitle);
        window.draw(topLine);
        window.draw(creditsInfo);
        window.draw(bottomLine);
        window.draw(backButton);
        window.draw(backText);
        window.display();
    }
}

void show_welcome_window(RenderWindow& window, Font& gameFont) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Title text setup
    Text titleText;
    titleText.setFont(gameFont);
    titleText.setString("Pong Game");
    titleText.setCharacterSize(70);
    titleText.setFillColor(Color::White);
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition((screenWidth - titleBounds.width) / 2, 100);

    // Buttons for Play, Exit, and Credits
    RectangleShape playButton(Vector2f(200, 50));
    playButton.setFillColor(Color::Green);
    playButton.setPosition(screenWidth / 2 - 100, screenHeight / 2 - 70);

    Text playText;
    playText.setFont(gameFont);
    playText.setString("Play");
    playText.setCharacterSize(30);
    playText.setFillColor(Color::White);
    playText.setPosition(playButton.getPosition().x + 70, playButton.getPosition().y + 5);

    RectangleShape exitButton(Vector2f(200, 50));
    exitButton.setFillColor(Color::Red);
    exitButton.setPosition(screenWidth / 2 - 100, screenHeight / 2 + 20);

    Text exitText;
    exitText.setFont(gameFont);
    exitText.setString("Exit");
    exitText.setCharacterSize(30);
    exitText.setFillColor(Color::White);
    exitText.setPosition(exitButton.getPosition().x + 70, exitButton.getPosition().y + 5);

    RectangleShape creditsButton(Vector2f(200, 50));
    creditsButton.setFillColor(Color::Blue);
    creditsButton.setPosition(screenWidth / 2 - 100, screenHeight / 2 + 110);

    Text creditsText;
    creditsText.setFont(gameFont);
    creditsText.setString("Credits");
    creditsText.setCharacterSize(30);
    creditsText.setFillColor(Color::White);
    creditsText.setPosition(creditsButton.getPosition().x + 45, creditsButton.getPosition().y + 5);

    // Event loop to handle Welcome screen interactions
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                exit(0);
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);

                // Check which button was clicked
                if (playButton.getGlobalBounds().contains(mousePosition)) {
                    show_mode_selectionwindow(window, gameFont);
                    return;
                }
                if (exitButton.getGlobalBounds().contains(mousePosition)) {
                    window.close();
                    exit(0);
                }
                if (creditsButton.getGlobalBounds().contains(mousePosition)) {
                    show_credits_window(window, gameFont);
                    return;
                }
            }
        }

        // Button hover effects
        Vector2f mousePosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
        playButton.setFillColor(playButton.getGlobalBounds().contains(mousePosition) ? Color::Yellow : Color::Green);
        exitButton.setFillColor(exitButton.getGlobalBounds().contains(mousePosition) ? Color::Magenta : Color::Red);
        creditsButton.setFillColor(creditsButton.getGlobalBounds().contains(mousePosition) ? Color::Cyan : Color::Blue);

        // Render Welcome screen
        window.clear();
        window.draw(titleText);
        window.draw(playButton);
        window.draw(playText);
        window.draw(exitButton);
        window.draw(exitText);
        window.draw(creditsButton);
        window.draw(creditsText);
        window.display();
    }
}

void restart_game(RenderWindow& window, Font& gameFont) {
    // Reset Timer Mode and clock
    is_timermode = false;
    game_clock.restart();

    // Reset paddle positions
    left_paddle.setPosition(30, 250);
    right_paddle.setPosition(760, 250);

    // Reset ball properties
    balls.clear();
    ballSpeedX.clear();
    ballSpeedY.clear();

    CircleShape ball(10);
    ball.setFillColor(Color::White);
    ball.setPosition(400, 300);
    balls.push_back(ball);
    ballSpeedX.push_back(3.0f);
    ballSpeedY.push_back(2.0f);

    // Reset scores
    left_score = 0;
    right_score = 0;


}
