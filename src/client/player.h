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
		virtual void die();
		virtual Player* clone() const;

		// Jump, if possible.
		void jump();

		// Begin/end moving in 'move' direction.
		// 0 = up, 1 = left, 2 = right.
		void beginMove(unsigned char move);
		void endMove(unsigned char move);

		// Get the position of the center of the player.
		MPosition getPosition() const;

		// Get the pose of the player.
		// 0 = left, 1 = right, 2 = left looking up, 3 = right looking up,
		// 4 = jumping left, 5 = jumping right, 6 = dead looking left,
		// 7 = dead looking right.
		int getPose() const;

		bool isDead() const { return dead; }

		void swap(Player& other);

	private:
		// Direction of the attempted movement.
		// For xmov, -1 = left, 1 = right, 0 = no direction.
		// For ymov, 1 = up, 0 = no direction.
		signed char xmov, ymov;

		// Bitmask of attempted movement directions, the bits mirroring the
		// meaning of the arguments given to beginMove/endMove.
		unsigned char mov;

		// Generate values of xmov, ymov from mov.
		void updateMove();

		// The direction the player is facing. It's the same as the last
		// non-zero 'xmov' value.
		signed char facing;

		// If the player is dead.
		bool dead;
};
