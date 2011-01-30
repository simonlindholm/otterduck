#include <algorithm>
#include <cassert>
#include "gamestate.h"

void GameState::handleEvent(Event::Type e) {
	if (e == Event::Restart) {
		// Allow restarts at any time.
		*this = *savedState;
	}
	else if (!dead) {
		if (e == Event::BeginLookUp) getPlayer().beginMove(0);
		else if (e == Event::EndLookUp) getPlayer().endMove(0);
		else if (e == Event::BeginMoveLeft) getPlayer().beginMove(1);
		else if (e == Event::EndMoveLeft) getPlayer().endMove(1);
		else if (e == Event::BeginMoveRight) getPlayer().beginMove(2);
		else if (e == Event::EndMoveRight) getPlayer().endMove(2);
		else if (e == Event::Jump) getPlayer().jump();
	}
}

void GameState::setSavedState(const GameState* state) {
	savedState = state;
}

void GameState::step(unsigned int delay) {
	getPlayer().step(*this, delay);
}

void GameState::playerDie() {
	dead = true;
}

void GameState::drawObjects(UI& ui) const {
	getPlayer().draw(*this, ui);
}

GameState::GameState(const Map& map)
	: dead(false), map(map), pl(new Player(map))
{}

void GameState::swap(GameState& other) {
	std::swap(dead, other.dead);
	assert(&map == &other.map);
	std::swap(savedState, other.savedState);
	items.swap(other.items);
	pl.swap(other.pl);
}

GameState& GameState::operator=(const GameState& other) {
	GameState copy(other);
	swap(copy);
	return *this;
}
