#include "cl.h"
#include "gamestate.h"

GameState::Event GameState::Event::makeKeydown(int key) {
	Event ret;
	ret.type = Keydown;
	ret.key = key;
	return ret;
}

GameState::Event GameState::Event::makeKeyup(int key) {
	Event ret;
	ret.type = Keyup;
	ret.key = key;
	return ret;
}

void GameState::handleEvent(Event e) {
	if (e.type == Event::Keyup || e.type == Event::Keydown) {
		bool down = (e.type == Event::Keydown);

		if (e.key == CL_KEY_LEFT) heldKeys[KEY_LEFT] = down;
		if (e.key == CL_KEY_RIGHT) heldKeys[KEY_RIGHT] = down;
		if (e.key == CL_KEY_UP) heldKeys[KEY_UP] = down;
		if (e.key == CL_KEY_DOWN) heldKeys[KEY_DOWN] = down;
		xmov = heldKeys[KEY_RIGHT] - heldKeys[KEY_LEFT];
		ymov = heldKeys[KEY_DOWN] - heldKeys[KEY_UP];
		pl.setHeld(xmov, ymov);

		if (e.type == Event::Keydown && e.key == CL_KEY_Z) pl.jump();
	}
}

void GameState::step(unsigned int delay) {
	pl.step(*this, delay);
}

void GameState::drawObjects(UI& ui) const {
	pl.draw(*this, ui);
}

GameState::GameState(const Map& map)
	: map(map), pl(map)
{
	for (int i = 0; i < KEY_LAST; ++i)
		heldKeys[i] = false;
}
