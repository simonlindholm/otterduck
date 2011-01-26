#pragma once
#include "resources.h"

// An animation.
class Anim {
	public:
		Anim(Resources::Img start, unsigned int len, unsigned int delay);

		// Initialize Anim with an empty state. Calls to getImage will get
		// undefined results until operator= or resetFromDifferent is called.
		Anim();

		// Assign to this animation from another, resetting animation state
		// if it differs.
		void resetFromDifferent(const Anim& other);

		// Get the current image.
		Resources::Img getImage() const;

		// Step the animation.
		void step(unsigned int delay);

	private:
		Resources::Img start;
		int len, frame;
		int fdelay, left;
};
