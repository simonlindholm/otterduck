#include <iostream>
#include <cmath>
#include "player.h"
#include "ui.h"

namespace {
	const Coord xvelMax = 100000;
	const Coord yvelJump = -260000;
	const Coord yvelMax = 300000;
	const Coord yaccGravity = 900;
	const Coord xaccAir = 1000;
	const Coord xaccAirResistance = 200;
	const Coord xaccGround = 3000;
	const Coord xaccGroundResistance = 1500;

	const Coord playerWidth = 7400;
	const Coord playerHeight = 7400;
}

Player::Player(const Map& map)
	: PhysicsObject(map, Rect(20000, 30000, playerWidth, playerHeight))
{
	xmov = ymov = 0;
	facing = 1;
}

void Player::jump() {
	if (ground) {
		yvel = yvelJump;
		ground = false;
	}
}

void Player::setHeld(signed char xmov, signed char ymov) {
	this->xmov = xmov;
	this->ymov = ymov;
	if (xmov) facing = xmov;
}

void Player::framePhysics() {
	if (!ground) {
		// Apply gravity.
		yvel += yaccGravity;
	}

	// Apply acceleration in x direction
	Coord xacc = (ground ? xaccGround : xaccAir);
	Coord xaccResistance = (ground ? xaccGroundResistance : xaccAirResistance);

	xvel += xacc * xmov;
	Coord ns = std::abs(xvel) - xaccResistance;
	if (ns < 0) ns = 0;
	if (xvel < 0) xvel = -ns;
	else xvel = ns;

	// Limit the possible velocity
	if (xvel < -xvelMax) xvel = -xvelMax;
	if (xvel > +xvelMax) xvel = +xvelMax;
	if (yvel < -yvelMax) yvel = -yvelMax;
	if (yvel > +yvelMax) yvel = +yvelMax;
}

void Player::step(GameState& gs, unsigned int delay) {
	stepPhysics(delay);
}

void Player::draw(const GameState& gs, UI& ui) const {
	ui.drawPlayer(*this);
}

Player* Player::clone() const {
	return new Player(*this);
}

int Player::getPose() const {
	int ret = 0;
	ret |= (facing > 0 ? 1 : 0);
	if (!ground) ret |= 4;
	else if (ymov < 0) ret |= 2;
	return ret;
}

Position Player::getPosition() const {
	return rect.getCenter();
}
