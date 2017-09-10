#include <iostream>
#include <ctime>
#include <SFML/Window.hpp>
#include "gamestate.h"
#include "map.h"
#include "ui.h"

enum {
	timestep = 10,
};

unsigned int getMs(const sf::Clock& cl) {
	return (unsigned int)(cl.getElapsedTime().asMilliseconds());
}

int main() {
	try {
		// Set up display, resources, etc.
		sf::RenderWindow window(sf::VideoMode(640, 480),
				"The Cracking Good Adventures of Otter and Duck", sf::Style::Close);

		ResourceManager resources;

		// Create a saved and a current game object, with a test map.
		// (In the future, there should be a menu here first, and the
		// creation of the game object should be handled somewhere in the
		// state machine.)
		Map map("maps/test.map");
		GameState gs(&map);

		UI ui(&resources, &window);

		std::queue<GameState::Event::Type> eventQueue;
		window.setKeyRepeatEnabled(false);

		// Run the game loop and relevant logic
		// (In the future, Application should probably only contain a state
		// machine, delegating the game loop logic to somewhere else.)

		sf::Clock clock;
		sf::Clock fpsClock;
		int fpsCount = 0;
		unsigned int ptime = getMs(clock);
		sf::Event event;
		bool quit = false;

		for (;;) {
			unsigned int ntime = getMs(clock);
			int dif = ntime - ptime, odif = dif;

			// Poll for new events.
			while (window.pollEvent(event)) {
				// Handle quitting locally, because it's simpler that way.
				if (event.type == sf::Event::Closed ||
						(event.type == sf::Event::KeyPressed &&
						 event.key.code == sf::Keyboard::Escape)) quit = true;
				ui.makeEvent(event, gs, eventQueue);
			}
			if (quit) break;

			// Send information about all events that have happened.
			while (!eventQueue.empty()) {
				GameState::Event::Type e = eventQueue.front();
				gs.handleEvent(e);
				eventQueue.pop();
			}

			// Step the gamestate in 'timestep' large steps.
			while (dif > (int)timestep) {
				dif -= timestep;
				gs.step(timestep);
			}
			if (dif > 0) {
				gs.step((unsigned int)dif);
				ptime = ntime;
			}

			// Calculate the actual time stepped.
			if (odif < 0) odif = 0;

			// Draw the interface to screen.
			ui.paint((unsigned int)odif, gs);

			// Draw everything to screen.
			window.display();

			++fpsCount;
			if (fpsClock.getElapsedTime().asSeconds() >= 1) {
				// std::cout << "FPS: " << fpsCount << '\r' << std::flush;
				fpsCount = 0;
				fpsClock.restart();
			}
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
