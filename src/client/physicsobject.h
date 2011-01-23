#pragma once
#include "util.h"
#include "gameobject.h"

class Map;

// Specialized abstract game object with added physics.
class PhysicsObject : public RawGameObject {
	public:
		// Construct a physics object operating on a given map.
		PhysicsObject(const Map& map, Rect rect);

		// Swap two PhysicsObjects.
		void swap(PhysicsObject& other);

		// Perform all physics/movement. Should be called every frame.
		void stepPhysics(unsigned int delay);

		// Perform the relevant physics for a frame, such as increasing
		// velocity due to gravity. This is a pure virtual function since
		// the details of how velocities change is dependent on the actual
		// object. (For example, a player's x velocity is directly
		// correlated to the held movement keys.)
		virtual void framePhysics() = 0;

		// Signal that the object hit a lethal tile.
		virtual void die() {}

	protected:
		// Flag for whether the player is at the ground.
		// When set, yval may contain any value.
		bool ground;

		// The velocities given as pixels/1024ms.
		mcoord xvel, yvel;

	private:
		const Map& map;

		// A struct representing a beginning and end of tile indices
		// that the player occupies.
		struct TileRange {
			tcoord c1, c2;
			bool operator==(const TileRange& other) {
				return c1 == other.c1 && c2 == other.c2;
			}
			bool operator!=(const TileRange& other) {
				return !(*this == other);
			}
		};

		// The remainders of the velocities, described in stepPhysics().
		mcoord xvelRem, yvelRem;

		// The tile range block the player is within.
		TileRange xtr, ytr;

		// Get the tile range for an interval [pos, pos+size-1] occupied
		// by the player.
		TileRange getTileRange(mcoord pos, mcoord size) const;

		// Check if the current position is within a block.
		bool invalidPos() const;

		// Check if the player is directly above ground.
		bool calculateGround() const;

		// Convert a velocity per 1024ms to one per 1ms, with remainder.
		mcoord velPerMs(mcoord vel, mcoord* rem);
};
