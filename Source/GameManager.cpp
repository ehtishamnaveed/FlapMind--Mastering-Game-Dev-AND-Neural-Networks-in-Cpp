#include <SFML/Graphics.hpp>
#include "../Header/Game.h"
#include "../Header/GameManager.h"

namespace Game {
    // Constructor initializes GameManager with default score and font settings
    GameManager::GameManager(): Score(0) {
        // Load font from file
        ScoreFont.loadFromFile("Resources/Font/4Brain.ttf");

        // Set up ScoreText properties
        ScoreText.setFont(ScoreFont);
        ScoreText.setCharacterSize(60);
        ScoreText.setFillColor(sf::Color::Black);
        ScoreText.setPosition(Game::Screen::screenWidth/2-20 , Game::Screen::screenHeight/20-20);
        ScoreText.setString(std::to_string(Score));

    }

    void GameManager::aiGameplay(sf::RenderWindow& i_window) {
        // Generating Weights for each bird
        for (NeuralNetwork::AI& aibirds : BirdAI) {
            aibirds.generateWeights();
        }

        bool keyPressed = false;
        while (i_window.isOpen()) {
            i_window.clear();
            // Handle window closing event
            sf::Event event;
            while (i_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed){
                    Game::saveConfiguration();
                    i_window.close();
                    break;
                }
                else if (event.type == sf::Event::KeyPressed && !keyPressed && event.key.code == sf::Keyboard::E) {
                    keyPressed = true;  // Set the flag to true to indicate key press
                    this->setGameMode(GameModes::Easy);
                }
                else if (event.type == sf::Event::KeyPressed && !keyPressed && event.key.code == sf::Keyboard::H) {
                    keyPressed = true;  // Set the flag to true to indicate key press
                    this->setGameMode(GameModes::Hard);
                }
                else if (event.type == sf::Event::KeyPressed && !keyPressed && event.key.code == sf::Keyboard::C) {
                    keyPressed = true;  // Set the flag to true to indicate key press
                    this->setGameMode(GameModes::Crazy);
                }
                else if (event.type == sf::Event::KeyReleased) {
                    keyPressed = false;  // Reset the flag on key release
                }

            }
            // Draws Backgound
            drawBackground(i_window);

            // Updates and Draws Pipes
            PipeController.updatePipes();
            PipeController.drawPipes(i_window);

            // Ai controler
            controlAIBehaviour(i_window);

            // Displays Score
            i_window.draw(ScoreText);

            // Displays Window
            i_window.display();
        }
    }

    void GameManager::controlAIBehaviour(sf::RenderWindow& i_window) {
        bool needrestart = true;
        for (NeuralNetwork::AI& aibird : BirdAI) {
            if (aibird.isAlive()) {
                needrestart = false;
                break;
            }
        }

        if (!needrestart) {
            for (NeuralNetwork::AI& aibirds : BirdAI) {
                // AI (Updates and Draws itself)
                aibirds.playGame(i_window,PipeController.getPipes());
            }

            for (NeuralNetwork::AI& aibird : BirdAI) {
                // Check for Collision
                if (collisionOfBirdWithPipes(aibird, PipeController.getPipes())) {
                    // If collides, The bird Dies
                    aibird.Dies();
                }
            }
        }
        else {
            //std::sort will use the operator functions in the AI class.
            std::sort(std::begin(BirdAI), std::end(BirdAI), std::greater<>());

            // We won't change the weights of the first 2 birds since they're the BEST!
            for (size_t i = 2 ; i < PopulationSize ; ++i) {
                BirdAI[i].uniformCrossoverWithMutation(BirdAI[0].getWeights(), BirdAI[1].getWeights());
            }

            for (NeuralNetwork::AI& aibirds : BirdAI) {
                aibirds.resetState();
            }
            PipeController.resetPipes();
        }
    }

    // Play Score FX
    void GameManager::playScoreFX() {
        if (!Game::mute) {
            // Load sound files into sound buffers
            FXsound.loadFromFile("Resources/SoundFX/ScoreFX.mp3");

            // Associate sound buffers with sound objects
            FX.setBuffer(FXsound);
            FX.play();
        }
    }

    // Play LowScore FX
    void GameManager::playLowScoreFX() {
        if (!Game::mute) {
            // Load sound files into sound buffers
            FXsound.loadFromFile("Resources/SoundFX/LowScoreFX.mp3");

            // Associate sound buffers with sound objects
            FX.setBuffer(FXsound);
            FX.play();
        }
    }

    // Play HighScore FX
    void GameManager::playHighScoreFX() {
        if (!Game::mute) {
            // Load sound files into sound buffers
            FXsound.loadFromFile("Resources/SoundFX/HighScoreFX.mp3");

            // Associate sound buffers with sound objects
            FX.setBuffer(FXsound);
            FX.play();
        }
    }

    // Main game loop logic
    void GameManager::runGame(sf::RenderWindow& i_window) {
        // If the Bird is alive
        while (i_window.isOpen() && bird.isAlive()) {
            // Handle window closing event
            sf::Event event;
            while (i_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed){
                    Game::saveConfiguration();
                    i_window.close();
                    break;
                }
            }

            // Render the display including pipes, bird, and score
            renderGame(i_window);

            // Check for Collision
            if (collisionOfBirdWithPipes(bird, PipeController.getPipes())) {
                // If collides, The bird Dies
                bird.Dies();
            }
        }
    }


    // Draw pipes, bird, and score on the window
    void GameManager::renderGame(sf::RenderWindow& i_window) {
        i_window.clear();
        // Draw the Background
        drawBackground(i_window);
        // Updates Pipes position
        PipeController.updatePipes();
        // Displays Pipe
        PipeController.drawPipes(i_window);
        // Updates Bird Position
        bird.updateBird();
        // Displays Bird
        bird.drawBird(i_window);
        // Displays Score
        i_window.draw(ScoreText);
        // Display everything on the window
        i_window.display();
    }


    // Increment the score
    void GameManager::updateScore() {
        Score++;
        ScoreText.setString(std::to_string(Score));
    }

    // Collision Detection
    bool GameManager::collisionOfBirdWithPipes(const Bird& bird, const std::vector<Pipe>& pipes) {
        // Constants
        const unsigned char collisionAnomaly = 6;
        const short birdSize = bird.getBirdSize();

        // Bird Position Information
        const short birdXPos = bird.getXPosition();
        const short birdYPos = bird.getYPosition();

        // Bird Collision Boundaries Information
        const unsigned char birdLeftBoundary = birdXPos - birdSize;
        const unsigned char birdRightBoundary = birdXPos + birdSize;

        // Loop through pipes
        for (const Pipe& pipe : pipes) {
            // Pipe information
            const short pipeXPos = pipe.getXPosition();
            const short pipeYPos = pipe.getYPosition();
            const unsigned char pipeGapSize = pipe.getGapSize();
            const char pipeSpeed = pipe.getPipeSpeed();

            // Collision Range information
            const bool pipeInCollisionRange = pipeXPos < birdRightBoundary - collisionAnomaly && pipeXPos > birdLeftBoundary;
            const bool pipeIsBehindTheBird = pipeXPos < birdLeftBoundary - pipeSpeed;
            const bool birdPassedThePipe = birdXPos > pipeXPos + birdSize;

            if (pipeIsBehindTheBird)
                continue; // Continue to the next pipe then

            if (pipeInCollisionRange) {
                // Check for collision excluding the gap area
                if (birdYPos <= pipeYPos - collisionAnomaly || birdYPos + birdSize >= pipeYPos + pipeGapSize + collisionAnomaly)
                    return true; // Collision detected
                break;
            }

            else if (birdPassedThePipe) {
                updateScore();
                playScoreFX();
                break;
            }
        }
        return false; // No collision
    }

    // Reset the game state to start a new game
    void GameManager::resetGameState() {
        resetScore();
        bird.resetState();
        PipeController.resetPipes();
    }

    // Reset the score to zero and update the display
    void GameManager::resetScore() {
        Score = 0;
        ScoreText.setFillColor(sf::Color::Black);
        ScoreText.setString(std::to_string(Score));
        ScoreText.setCharacterSize(60);
        ScoreText.setPosition(Game::Screen::screenWidth/2-20 , Game::Screen::screenHeight/20-20);
    }

    // Draw the Backgound for Play Menu
    void GameManager::drawBackground(sf::RenderWindow& i_window) {
        background_texture.loadFromFile("Resources/Theme/"+Game::theme_name+"/Background.png");
        background_sprite.setTexture(background_texture);
        i_window.draw(background_sprite);
    }

    // It sets the Game Mode type
    void GameManager::setGameMode(const GameModes GameType) {
         PipeController.setSettings(GameType);
    }

    // End Game Overlay
    void GameManager::displayGameOverOverlay(sf::RenderWindow& window, unsigned short*& bestScore) {
        sf::Texture texture;
        sf::Sprite sprite;
        texture.loadFromFile("Resources/Images/Scoreboard.png");

        // Calculate texture rect size only once
        sf::IntRect rect;
            if (Score > *bestScore) {
                playHighScoreFX();
                *bestScore = Score;
                rect = sf::IntRect(300, 0, 300, 150);
            }
            else {
                playLowScoreFX();
                rect = sf::IntRect(0, 0, 300, 150);

                // Gamplay Score
                ScoreText.setCharacterSize(40);
                ScoreText.setPosition(350 , 205);
            }

        // Set texture rect, texture, and position for the sprite
        sprite.setTextureRect(rect);
            sprite.setTexture(texture);
            sprite.setPosition(Game::Screen::screenWidth/2-160, Game::Screen::screenHeight/2-100);
        // Draw the sprite
        window.draw(sprite);
        // Draw Gamplay Score
        window.draw(ScoreText);

        // Draw High Score
        ScoreText.setString(std::to_string(*bestScore));
            ScoreText.setCharacterSize(40);
            ScoreText.setPosition(350 , 255);
            window.draw(ScoreText);

        window.display();
    }

}