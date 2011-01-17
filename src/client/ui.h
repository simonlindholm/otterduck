#pragma once
#include "precomp.h"
#include "gamestate.h"

class UI {
	public:
		UI(CL_ResourceManager* res, CL_DisplayWindow* win);
		void paint(unsigned int delay, const GameState& gs);

	private:
		CL_ResourceManager* resources;
		CL_DisplayWindow* window;
		CL_GraphicContext gc;

		CL_Sprite otterLeft, otterRight;
		CL_Sprite otterUpLeft, otterUpRight;
		CL_Sprite otterDownLeft, otterDownRight;
		CL_Sprite otterSprite, tileSprite;

		Coord scrollx, scrolly;

		// Draw a sprite to the screen at a map-based position, adjusted
		// for scrolling.
		void drawMapSprite(CL_Sprite& spr, Position p);

		// Adjust scrolling based on the position of the player, so that
		// the player does not go out of view.
		void adjustScrolling(const Player& pl);
};
