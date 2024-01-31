#include <SFML/Graphics.hpp>
// #include <random>
#include <chrono>
#include "../Header/PipeManager.h"
#include "../Header/Game.h"
#include <iostream>

namespace Game {
	PipeManager::PipeManager(): 
		Y_Distribution(PipeIndent, Game::Screen::screenHeight - PipeIndent), 
		RandomEngine(std::random_device{}()),
		GeneratorTimer(150) {}

	// Draw Pipes on Screen
	void PipeManager::drawPipes(sf::RenderWindow& i_window) {
		for (Pipe& curr_pipe : Pipes){
			curr_pipe.draw(i_window);
		}
	}

	// Update Pipe positions and generate new location of Pipes
	void PipeManager::updatePipes() {
		if (0 == GeneratorTimer) {
			GeneratorTimer = 150;
			Pipes.push_back(Pipe(Game::Screen::screenWidth, Y_Distribution(RandomEngine)));
		}
		--GeneratorTimer;

		for (Pipe& curr_pipe : Pipes) {
			curr_pipe.update();
		}

		for (std::vector<Pipe>::iterator curr_pipe = Pipes.begin(); curr_pipe != Pipes.end(); curr_pipe++){
			// When the pipes go beyond the screen.
			if (curr_pipe->isRemoveable()){
				// We delete them.
				Pipes.erase(curr_pipe);
				break;
			}
		}
	}

	const std::vector<Pipe>& PipeManager::getPipes() const {
		return Pipes;
	}

	void PipeManager::resetPipes() {
		GeneratorTimer = 150;
		Pipes.clear();
	}
}