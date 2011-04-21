#include <algorithm>
#include <cassert>
#include "physicsobject.h"
#include "map.h"

PhysicsObject::PhysicsObject(const Map* map, Rect rect)
	: RawGameObject(rect), map(map)
{
	ground = false;
	xvel = yvel = 0;
	xvelRem = yvelRem = 0;
	xtr = getTileRange(rect.p.x, rect.w);
	ytr = getTileRange(rect.p.y, rect.h);
}

void PhysicsObject::stepPhysics(unsigned int delay) {
	/**
	 * The algorithm for moving is as follows:
	 * - For each 1ms delay:
	 *   - Perform physics such as gravity by delegating to the subclass.
	 *   - Move in x direction.
	 *   - If tile change:
	 *     - If the tiles are not passable, move to the edge of the new tile,
	 *       and set the x velocity to 0.
	 *     - Else, if the ground flag is set, find out if we actually are on
	 *       the ground and if not, reset the flag and initialize the
	 *       y velocity by setting it to 0.
	 *   - If the ground flag is not set, do the same for y direction, and
	 *     set the ground flag if the object is moved back after a downward
	 *     movement.
	 * This has the advantage of being simple and also easy to expand to
	 * arbitrary kinds of movements, and has the disadvantages of being
	 * slightly slower than the optimal implementation, and not being entirely
	 * exact and instead slightly favoring landing on the top of surfaces.
	 */

	for (unsigned int ts = 0; ts < delay; ++ts) {
		this->framePhysics();

		bool changedTile = false;

		// Move the object in the x direction. The x velocity per 1024ms is
		// converted (by something reassembling a simple division) to an
		// x velocity per 1ms. To avoid information losses from the reduction
		// in scale of the velocity, a remainder is kept and managed by
		// the conversion function; this remainder is ignored by the rest
		// of the code since the offset it represents is too insignificant
		// to be noticable.
		TileRange oldXtr = xtr;
		rect.p.x += velPerMs(xvel, &xvelRem);
		xtr = getTileRange(rect.p.x, rect.w);

		if (xtr != oldXtr) {
			if (invalidPos()) {
				// The new tile position is either directly to the left or
				// the right of the old one, and this easily gives us the
				// position we should move to to be at the edge of the tile
				// collided with.
				if (xtr.c1 < oldXtr.c1) {
					rect.p.x = Map::tileToPos(oldXtr.c1);
				}
				else {
					rect.p.x = Map::tileToPos(xtr.c2) - rect.w;
				}

				// Calculate the new tile range (this is likely the same
				// as 'oldXtr', but that is not guaranteed).
				xtr = getTileRange(rect.p.x, rect.w);

				// Assume a perfectly inelastical collision, so don't continue
				// any movement in x direction.
				xvel = 0;
			}
			else {
				changedTile = true;
				if (ground) {
					// Find out if the object has left the ground.
					if (!calculateGround()) {
						ground = false;
						yvel = 0;
					}
				}
			}
		}

		if (!ground) {
			TileRange oldYtr = ytr;
			rect.p.y += velPerMs(yvel, &yvelRem);
			ytr = getTileRange(rect.p.y, rect.h);

			if (ytr != oldYtr) {
				if (invalidPos()) {
					if (ytr.c1 < oldYtr.c1) {
						rect.p.y = Map::tileToPos(oldYtr.c1);
					}
					else {
						rect.p.y = Map::tileToPos(ytr.c2) - rect.h;
					}
					ytr = getTileRange(rect.p.y, rect.h);

					// If the object was moving downwards and hit something, it
					// must have been the ground, so set the ground flag.
					if (yvel > 0) ground = true;

					yvel = 0;
				}
				else {
					changedTile = true;
				}
			}
		}
		if (changedTile) {
			// Perform checks against deadly tiles
			bool dead = false;
			for (tcoord x = xtr.c1; x <= xtr.c2; ++x) {
				for (tcoord y = ytr.c1; y <= ytr.c2; ++y) {
					if (map->getTileAt(TPosition(x,y)).isDeadly()) {
						dead = true;
						break;
					}
				}
			}
			if (dead) this->die();
		}
	}
}

mcoord PhysicsObject::velPerMs(mcoord vel, mcoord* rem) {
	const int div = 1024;
	*rem += vel;
	mcoord nrem = *rem;
	*rem &= (div-1);

	// Round 'nrem' down to the nearest multiple of 'div' to avoid
	// rounding towards zero instead of flooring.
	nrem -= *rem;

	return nrem / div;
}

bool PhysicsObject::calculateGround() const {
	tcoord y = ytr.c2 + 1;
	for (tcoord x = xtr.c1; x <= xtr.c2; ++x) {
		if (!map->getTileAt(TPosition(x, y)).isEmpty()) return true;
	}
	return false;
}

bool PhysicsObject::invalidPos() const {
	for (tcoord x = xtr.c1; x <= xtr.c2; ++x) {
		for (tcoord y = ytr.c1; y <= ytr.c2; ++y) {
			if (!map->getTileAt(TPosition(x,y)).isEmpty()) return true;
		}
	}
	return false;
}

PhysicsObject::TileRange PhysicsObject::getTileRange(mcoord pos, mcoord size) const {
	TileRange ret;
	ret.c1 = Map::posToTile(pos);
	ret.c2 = Map::posToTile(pos + size-1);
	return ret;
}
