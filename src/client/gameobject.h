#pragma once
#include <memory>
#include "util.h"

class GameState;
class Player;
class RawGameObject;
class UI;

// An object that appears on the map and can be interacted with, such as
// a save point or an item. Has value semantics.
class GameObject {
	public:
		// A player tries to use the object by pressing C on it. Returns true
		// if the attempt was valid, or false if it should instead continue
		// to try objects in the background.
		bool use(GameState& gs, Player& pl);

		// Draw the object to screen by delegating to a function in UI.
		void draw(const GameState& gs, UI& ui) const;

		// Step the logic by 'delay' ms.
		void step(GameState& gs, unsigned int delay);

		// Check whether a point is within the object.
		bool within(Position p) const;

		GameObject(const GameObject& other);
		GameObject(GameObject&& other);
		GameObject& operator=(const GameObject& other);
		void swap(GameObject& other);

		GameObject(RawGameObject* raw);

		// Convert the game object into a known subtype. We assume that no
		// virtual inheritance will occur, so we can use static_cast for this.
		template<class T>
		T& getAs() { return static_cast<T&>(*obj); }
		template<class T>
		const T& getAs() const { return static_cast<const T&>(*obj); }

	private:
		std::shared_ptr<RawGameObject> obj;
};

// Abstract class for implementing the polymorphic backend of GameObject.
// RawGameObject is the base class of every actual game object, and mirrors
// the interface of GameObject.
class RawGameObject {
	public:
		virtual bool use(GameState& gs, Player& pl) { return false; }
		virtual void draw(const GameState& gs, UI& ui) const = 0;
		virtual void step(GameState& gs, unsigned int delay) = 0;
		virtual bool within(Position p) const;

		// Create a copy of the object, or NULL if no new object is needed
		// (like for instance checkpoints, which keep no state).
		virtual RawGameObject* clone() const = 0;

		virtual ~RawGameObject() {}

	protected:
		// The position and dimensions of the object. Can be changed freely
		// from derived classes.
		Rect rect;

		// Initialize the base part of a game object with a given rect.
		RawGameObject(Rect rect);
};
