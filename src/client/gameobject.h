#pragma once
#include <memory>
#include "util.h"

class GameState;
class Player;
class RawGameObject;
class UI;

// Abstract class for implementing the polymorphic backend of GameObject.
// RawGameObject is the base class of every actual game object, and mirrors
// the interface of GameObject.
class RawGameObject {
	public:
		// A player tries to use the object by pressing C on it. Returns true
		// if the attempt was valid, or false if it should instead continue
		// to try objects in the background.
		virtual bool use(GameState& gs, Player& pl) { return false; }

		// Draw the object to screen by delegating to a function in UI.
		virtual void draw(const GameState& gs, UI& ui) const = 0;

		// Step the logic by 'delay' ms.
		virtual void step(GameState& gs, unsigned int delay) = 0;

		// Check whether a point is within the object.
		virtual bool within(MPosition p) const;

		// Create a copy of the object.
		virtual RawGameObject* clone() const = 0;

		// Assignment from an object known to be of the same dynamic type.
		virtual void setFrom(RawGameObject& other) = 0;

		virtual ~RawGameObject() {}

	protected:
		// The position and dimensions of the object. Can be changed freely
		// from derived classes.
		Rect rect;

		// Initialize the base part of a game object with a given rect.
		RawGameObject(Rect rect);
};
