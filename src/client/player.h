#pragma once
#include "util.h"
#include "physicsobject.h"

class Map;

// A user-controlled player; either Otter or Duck.
class Player : public PhysicsObject {
	public:
		Player(const Map& map);

		virtual void step(GameState& gs, unsigned int delay);
		virtual void draw(const GameState& gs, UI& ui) const;
		virtual void framePhysics();
		virtual Player* clone() const;

		// Jump, if possible.
		void jump();

		// Set the direction in which the user holds the x/y movement keys.
		// For xmov, -1 = left, 1 = right, 0 = no direction.
		// For ymov, -1 = up, 1 = down, 0 = no direction.
		void setHeld(signed char xmov, signed char ymov);

		// Get the position of the center of the player.
		Position getPosition() const;

		// Get the pose of the player.
		// 0 = left, 1 = right, 2 = left looking up, 3 = right looking up,
		// 4 = jumping left, 5 = jumping right.
		int getPose() const;

	private:
		// Direction of the movement keys, as given by setHeld().
		signed char xmov, ymov;

		// The direction the player is facing. It's the same as the last
		// non-zero 'xmov' value.
		signed char facing;
};
