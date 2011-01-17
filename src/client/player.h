#pragma once
#include "precomp.h"
#include "util.h"
#include "map.h"

// A user-controlled player; either Otter or Duck.
class Player {
	public:
		// Construct a player
		Player(const Map& map);

		// Jump, if possible.
		void jump();

		// Perform all physics/movement. Should be called every frame.
		void move(unsigned int delay);

		// Set the direction in which the user holds the x/y movement keys.
		// For xmov, -1 = left, 1 = right, 0 = no direction.
		// For ymov, -1 = up, 1 = down, 0 = no direction.
		void setHeld(signed char xmov, signed char ymov);

		// Get the position of the player.
		Position getPosition() const { return pos; }

		Coord getWidth() const;
		Coord getHeight() const;

		// Get the pose of the player.
		// 0 = left, 1 = right, 2 = left looking up, 3 = right looking up,
		// 4 = jumping left, 5 = jumping right.
		int getPose() const;

	private:
		Position pos;
		const Map& map;

		// Flag for whether the player is at the ground.
		// When set, yval may contain any value.
		bool ground;

		// The velocities given as pixels/1024ms.
		Coord xvel, yvel;

		// Their remainders, described in move().
		Coord xvelRem, yvelRem;

		// Direction of the movement keys, as given by setHeld().
		signed char xmov, ymov;

		// The direction the player is facing. It's the same as the last
		// non-zero 'xmov' value.
		signed char facing;

		// A struct representing a beginning and end of tile indices
		// that the player occupies.
		struct TileRange {
			Coord c1, c2;
			bool operator==(const TileRange& other) { return c1 == other.c1 && c2 == other.c2; }
			bool operator!=(const TileRange& other) { return !(*this == other); }
		};

		// The tile range block the player is within.
		TileRange xtr, ytr;

		// Get the tile range for an interval [pos, pos+size-1] occupied
		// by the player.
		TileRange getTileRange(Coord pos, Coord size) const;

		// Check if the current position is within a block.
		bool invalidPos() const;

		// Check if the player is directly above ground.
		bool calculateGround() const;

		// Convert a velocity per 1024ms to one per 1ms, with remainder.
		Coord velPerMs(Coord vel, Coord* rem);
};
