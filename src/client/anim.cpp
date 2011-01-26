#include "anim.h"

Anim::Anim(Resources::Img start, unsigned int len, unsigned int delay)
	: start(start), len(len), frame(0), fdelay(delay), left(delay)
{}

Anim::Anim()
	: start((Resources::Img)-1), len(-1), frame(0), fdelay(0), left(0)
{}

void Anim::step(unsigned int delay) {
	if (len <= 0) return; // Don't do anything if empty.
	left -= delay;
	while (left <= 0) {
		++frame;
		if (frame == len) frame = 0;
		left += fdelay;
	}
}

Resources::Img Anim::getImage() const {
	return Resources::Img((int)start + frame);
}

void Anim::resetFromDifferent(const Anim& other) {
	if (start == other.start && len == other.len) return;
	*this = other;
}
