#include <iostream>
#include <cmath>
#include "player.h"
#include "ui.h"

namespace {
	const mcoord xvelMax = 100000;
	const mcoord yvelJump = -260000;
	const mcoord yvelMax = 300000;
	const mcoord yaccGravity = 900;
	const mcoord xaccAir = 1000;
	const mcoord xaccAirResistance = 200;
	const mcoord xaccGround = 3000;
	const mcoord xaccGroundResistance = 1500;

	const mcoord playerWidth = 7400;
	const mcoord playerHeight = 7400;
}

Player::Player(const Map& map)
	: PhysicsObject(map, Rect(20000, 30000, playerWidth, playerHeight)),
	xmov(0), ymov(0), mov(0), facing(1), dead(false)
{}

void Player::swap(Player& other) {
	std::swap(dead, other.dead);
	std::swap(facing, other.facing);
	std::swap(ymov, other.ymov);
	std::swap(xmov, other.xmov);
	std::swap(mov, other.mov);
	PhysicsObject::swap(other);
}

void Player::jump() {
	if (ground) {
		yvel = yvelJump;
		ground = false;
	}
}

void Player::beginMove(unsigned char move) {
	mov |= (1 << move);
	updateMove();
}

void Player::endMove(unsigned char move) {
	mov &= ~(1 << move);
	updateMove();
}

void Player::updateMove() {
	xmov = ((mov & 4)?1:0) - ((mov & 2)?1:0);
	ymov = (mov & 1)?1:0;
	if (xmov) facing = xmov;
}

void Player::framePhysics() {
	if (!ground) {
		// Apply gravity.
		yvel += yaccGravity;
	}

	// Apply acceleration in x direction
	mcoord xacc = (ground ? xaccGround : xaccAir);
	mcoord xaccResistance = (ground ? xaccGroundResistance : xaccAirResistance);

	xvel += xacc * xmov;
	mcoord ns = std::abs(xvel) - xaccResistance;
	if (ns < 0) ns = 0;
	if (xvel < 0) xvel = -ns;
	else xvel = ns;

	// Limit the possible velocity
	if (xvel < -xvelMax) xvel = -xvelMax;
	if (xvel > +xvelMax) xvel = +xvelMax;
	if (yvel < -yvelMax) yvel = -yvelMax;
	if (yvel > +yvelMax) yvel = +yvelMax;
}

void Player::die() {
	dead = true;

	// Make the player continue moving as if no movement keys were held.
	xmov = ymov = mov = 0;
}

void Player::step(GameState& gs, unsigned int delay) {
	stepPhysics(delay);
	if (dead) gs.playerDie();
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
	if (dead) ret += 6;
	else if (!ground) ret += 4;
	else if (ymov < 0) ret += 2;
	return ret;
}

MPosition Player::getPosition() const {
	return rect.getCenter();
}
