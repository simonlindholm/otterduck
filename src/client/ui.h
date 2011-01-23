#pragma once
#include <queue>
#include "cl.h"
#include "util.h"
#include "gamestate.h"

typedef long scoord;

class UI {
	public:
		struct Event {
			enum Type {
				Keydown,
				Keyup,
			};
			Type type;
			int key;
			static Event makeKeydown(int key);
			static Event makeKeyup(int key);
		};

		UI(CL_ResourceManager* res, CL_DisplayWindow* win);
		void paint(unsigned int delay, const GameState& gs);

		// Transform a raw keyboard/mouse event into the relevant game event
		// (if any), and add it to the event queue.
		void makeEvent(Event e, const GameState& gs,
				std::queue<GameState::Event::Type>& eventQueue) const;

		void drawPlayer(const Player& pl);

	private:
		CL_ResourceManager* resources;
		CL_DisplayWindow* window;
		CL_GraphicContext gc;

		CL_SpanLayout restartText, restartHeader;

		CL_Sprite otterLeft, otterRight;
		CL_Sprite otterUpLeft, otterUpRight;
		CL_Sprite otterDownLeft, otterDownRight;
		CL_Sprite otterSprite, tileSprite;

		CL_SoundBuffer soundDeath;
		bool hasPlayedDeathSound;

		scoord scrollx, scrolly;

		// Draw a sprite to the screen at a map-based position, adjusted
		// for scrolling.
		void drawMapSprite(CL_Sprite& spr, MPosition p);

		// Same as above, but the position provided is the center of the
		// sprite instead of the top-left.
		void drawCenteredMapSprite(CL_Sprite& spr, MPosition p);

		// Adjust scrolling based on the position of the player, so that
		// the player does not go out of view.
		void adjustScrolling(const Player& pl);

		// Layout a SpanLayout with no wrapping and centered x position.
		void layoutCentered(CL_SpanLayout& layout, int y);
};
