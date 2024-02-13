#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Bird.h"
#include "PipeManager.h"
#include "GameMode.h"

namespace Game {
    class GameManager {
    public:
        GameManager();
        // Sound FX
        void playScoreFX(); // FX when user scores
        void playLowScoreFX(); // FX for when user scores lower than the High score
        void playHighScoreFX(); // FX for when user scores higher than the High score

        void renderGame(sf::RenderWindow& i_window); // Displays the Bird,Pipes, and Backgound
        void runGame(sf::RenderWindow& i_window); // It is the main game loop logic
        bool collisionOfBirdWithPipes(const Bird& bird, const std::vector<Pipe>& Pipes); // Checks collision of Bird with Pipes
        void drawBackground(sf::RenderWindow& i_window); // Draw the backgound for Game

        void updateScore(); // Updates the score of the user
        void resetGameState(); // Reset the Game settings
        void resetScore(); // Reset the game score to '0'
        void setGameMode(const GameModes GameType); // It sets the Game Mode type
        void displayGameOverOverlay(sf::RenderWindow& window, unsigned short*& bestScore); // End Game Overlay

    private:
        Bird bird;
        PipeManager PipeController;

        // Score Attributes
        unsigned short Score;
        sf::Text ScoreText;
        sf::Font ScoreFont;

        sf::Texture background_texture;
        sf::Sprite background_sprite;

        // Sound FX
        sf::SoundBuffer FXsound;
        sf::Sound FX;
    };
}

#endif