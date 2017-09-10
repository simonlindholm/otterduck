#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <queue>
#include "util.h"
#include "anim.h"
#include "gamestate.h"
#include "resources.h"

typedef long scoord;

class UI {
	public:
		UI(ResourceManager* res, sf::RenderWindow* win);
		void paint(unsigned int delay, const GameState& gs);

		// Transform a raw keyboard/mouse event into the relevant game event
		// (if any), and add it to the event queue.
		void makeEvent(sf::Event e, const GameState& gs,
				std::queue<GameState::Event::Type>& eventQueue) const;

		void drawPlayer(const Player& pl);

	private:
		ResourceManager* resources;
		sf::RenderWindow* window;
		sf::Vector2f winDimensions;

		Anim otterAnim, otterUpLeft, otterLeft, otterDownLeft;

		sf::Text restartText, restartHeader;

		bool hasPlayedDeathSound;
		sf::Sound deathSound;

		scoord scrollx, scrolly;

		// Draw a sprite to the screen at a map-based position, adjusted
		// for scrolling.
		void drawMapSprite(sf::Sprite& spr, MPosition p);

		// Same as above, but the position provided is the center of the
		// sprite instead of the top-left.
		void drawCenteredMapSprite(sf::Sprite& spr, MPosition p, bool flipX);

		// Adjust scrolling based on the position of the player, so that
		// the player does not go out of view.
		void adjustScrolling(const Player& pl);

		// Layout a SpanLayout with no wrapping and centered x position.
		void centerText(sf::Text& str, int y);
};
