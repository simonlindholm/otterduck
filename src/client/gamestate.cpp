#include <algorithm>
#include <cassert>
#include "gamestate.h"

void GameState::handleEvent(Event::Type e) {
	if (e == Event::Restart) {
		// Allow restarts at any time.
		items.popState();
		items.pushState();
		data = savedData;
	}
	else if (!data.dead) {
		if (e == Event::BeginLookUp) pl->beginMove(0);
		else if (e == Event::EndLookUp) pl->endMove(0);
		else if (e == Event::BeginMoveLeft) pl->beginMove(1);
		else if (e == Event::EndMoveLeft) pl->endMove(1);
		else if (e == Event::BeginMoveRight) pl->beginMove(2);
		else if (e == Event::EndMoveRight) pl->endMove(2);
		else if (e == Event::Jump) pl->jump();
	}
}

void GameState::step(unsigned int delay) {
	for (auto it = items.begin(); it != items.end(); ++it) {
		items.markDirty(it);
		it->step(*this, delay);
	}
}

void GameState::playerDie() {
	data.dead = true;
}

void GameState::drawObjects(UI& ui) const {
	for (auto it = items.begin(); it != items.end(); ++it) {
		it->draw(*this, ui);
	}
}

GameState::GameState(const Map* map)
	: map(map)
{
	pl = new Player(map);
	items.push_back(std::shared_ptr<RawGameObject>(pl));
	items.pushState();

	data.dead = false;
	savedData = data;
}

void GameState::markDirty(RawGameObject* item) {
	items.markDirty(item);
}
