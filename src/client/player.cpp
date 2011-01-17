#include "precomp.h"
#include <iostream>
#include <cmath>
#include "player.h"

namespace {
	const Coord xvelMax = 100000;
	const Coord yvelJump = -260000;
	const Coord yvelMax = 300000;
	const Coord yaccGravity = 900;
	const Coord xaccAir = 1000;
	const Coord xaccAirResistance = 200;
	const Coord xaccGround = 3000;
	const Coord xaccGroundResistance = 1500;

	const Coord playerSize = 8000;
}

Player::Player(const Map& map)
	: map(map)
{
	pos.x = 20000;
	pos.y = 30000;
	ground = false;
	xvel = yvel = 0;
	xvelRem = yvelRem = 0;
	xmov = ymov = 0;
	xtr = getTileRange(pos.x, playerSize);
	ytr = getTileRange(pos.y, playerSize);
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

void Player::move(unsigned int delay) {
	/**
	 * The algorithm for moving is as follows:
	 * - For each 1ms delay:
	 *   - Perform physics such as changing y velocity due to gravity, and
	 *     handle jumping.
	 *   - Move in x direction.
	 *   - If tile change:
	 *     - If the tiles are not passable, move the player to the edge of
	 *       the new tile, and set the x velocity to 0.
	 *     - Else, if the ground flag is set, find out if we actually are
	 *       on the ground and if not, reset the flag and initialize the
	 *       y velocity by setting it to 0.
	 *   - If the ground flag is not set, do the same for y direction,
	 *     and set the ground flag if the player is moved back after a
	 *     downward movement.
	 * This has the advantage of being simple and also easy to expand to
	 * non-linear movements, and has the disadvantages of being slightly
	 * slower than the optimal implementation, and not being entirely
	 * exact and instead slightly favoring landing on the top of surfaces.
	 */

	for (unsigned int ts = 0; ts < delay; ++ts) {
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

		// Move the player in the x direction. The x velocity per 1024ms is
		// converted (by something reassembling a simple division) to an
		// x velocity per 1ms. To avoid information losses from the reduction
		// in scale of the velocity, a remainder is kept and managed by
		// the conversion function; this remainder is ignored by the rest
		// of the code since the offset it represents is too insignificant
		// to be noticable.
		TileRange oldXtr = xtr;
		pos.x += velPerMs(xvel, &xvelRem);
		xtr = getTileRange(pos.x, playerSize);

		if (xtr != oldXtr) {
			if (invalidPos()) {
				// The new tile position is either directly to the left or
				// the right of the old one, and this easily gives us the
				// position we should move to to be at the edge of the tile
				// collided with.
				if (xtr.c1 < oldXtr.c1) {
					pos.x = Map::tileToPos(oldXtr.c1);
				}
				else {
					pos.x = Map::tileToPos(xtr.c2) - playerSize;
				}

				// Calculate the new tile range (this is likely the same
				// as 'oldXtr', but that is not guaranteed).
				xtr = getTileRange(pos.x, playerSize);

				// Assume a perfectly inelastical collision, so don't continue
				// any movement in x direction.
				xvel = 0;
			}
			else if (ground) {
				// Find out if the player has left the ground.
				if (!calculateGround()) {
					ground = false;
					yvel = 0;
				}
			}
		}

		if (!ground) {
			TileRange oldYtr = ytr;
			pos.y += velPerMs(yvel, &yvelRem);
			ytr = getTileRange(pos.y, playerSize);

			if (ytr != oldYtr && invalidPos()) {
				if (ytr.c1 < oldYtr.c1) {
					pos.y = Map::tileToPos(oldYtr.c1);
				}
				else {
					pos.y = Map::tileToPos(ytr.c2) - playerSize;
				}
				ytr = getTileRange(pos.y, playerSize);

				// If the player was moving downwards and hit something, it
				// must have been the ground, so set the ground flag.
				if (yvel > 0) ground = true;

				yvel = 0;
			}
		}
	}
}

Coord Player::velPerMs(Coord vel, Coord* rem) {
	const int div = 1024;
	*rem += vel;
	Coord nrem = *rem;
	*rem &= (div-1);

	// Round 'nrem' down to the nearest multiple of 'div' to avoid
	// rounding towards zero instead of flooring.
	nrem -= *rem;

	return nrem / div;
}

bool Player::calculateGround() const {
	Coord y = ytr.c2 + 1;
	for (Coord x = xtr.c1; x <= xtr.c2; ++x) {
		if (!map.getTileAt(Position(x, y)).isEmpty()) return true;
	}
	return false;
}

bool Player::invalidPos() const {
	for (Coord x = xtr.c1; x <= xtr.c2; ++x) {
		for (Coord y = ytr.c1; y <= ytr.c2; ++y) {
			Position pos;
			pos.x = x;
			pos.y = y;
			if (!map.getTileAt(pos).isEmpty()) return true;
		}
	}
	return false;
}

int Player::getPose() const {
	int ret = 0;
	ret |= (facing >= 0 ? 1 : 0);
	if (!ground) ret |= 4;
	else if (ymov < 0) ret |= 2;
	return ret;
}

Coord Player::getWidth() const {
	return playerSize;
}

Coord Player::getHeight() const {
	return playerSize;
}

Player::TileRange Player::getTileRange(Coord pos, Coord size) const {
	TileRange ret;
	ret.c1 = Map::posToTile(pos);
	ret.c2 = Map::posToTile(pos + size-1);
	return ret;
}
