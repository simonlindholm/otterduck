#include <algorithm>
#include <cassert>
#include "gamestate.h"

void GameState::handleEvent(Event::Type e) {
	if (e == Event::Restart) {
		// Allow restarts at any time.
		*this = *savedState;
	}
	else if (!dead) {
		if (e == Event::BeginLookUp) pl.beginMove(0);
		else if (e == Event::EndLookUp) pl.endMove(0);
		else if (e == Event::BeginMoveLeft) pl.beginMove(1);
		else if (e == Event::EndMoveLeft) pl.endMove(1);
		else if (e == Event::BeginMoveRight) pl.beginMove(2);
		else if (e == Event::EndMoveRight) pl.endMove(2);
		else if (e == Event::Jump) pl.jump();
	}
}

void GameState::setSavedState(const GameState* state) {
	savedState = state;
}

void GameState::step(unsigned int delay) {
	pl.step(*this, delay);
}

void GameState::playerDie() {
	dead = true;
}

void GameState::drawObjects(UI& ui) const {
	pl.draw(*this, ui);
}

GameState::GameState(const Map& map)
	: dead(false), map(map), pl(map)
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
